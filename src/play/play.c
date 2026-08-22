#include "play.h"
#include "config.h"
#include "retro_bridge.h"
#include "playpause.h"
#include "controller.h"
#include "raylib.h"
#include "var.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ui/ui.h"
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>

//Key and pad maps
int your_key_map[RETRO_DEVICE_ID_JOYPAD_R3 + 1];
int your_pad_map[RETRO_DEVICE_ID_JOYPAD_R3 + 1];
//Textures for drawing the games
Texture2D emulator_texture;
//Time on the current frame
static double accumulator;
//FPS of the game
static double core_fps;
//Current state of the game after being launched
static PlayState currentPlayState;
static PlayState pauseState;
//Time between saves
static float saveTimeElapsed;
//Variables for resuming game after being paused
static bool resuming;
static float resumeTimer;
//Whether the game is running or not
static bool is_game_running;
//Variables for killing external application
static float killRequestedTime;
static bool killEscalated;
//Paths for config files
static char dolphinIniPath[CONFIG_PATH_LEN];
static char melonDSPath[CONFIG_PATH_LEN];
static char ppssppControlsPath[CONFIG_PATH_LEN];
static char ppssppIniPath[CONFIG_PATH_LEN];
static char flycastEmuPath[CONFIG_PATH_LEN];
static char saturnPath[CONFIG_PATH_LEN];

//Used for hardware rendering
static RenderTexture2D hw_target = {0}; 
//Whether these cores are active
static bool g_isDoomActive = false;
static bool g_isN64Active  = false;
//ID for external application
static pid_t extAppId = -1;

//Build path for the config files
static bool Play_BuildConfigPath(char* outPath, size_t outSize, const char* relativePath) {
    const char* home = getenv("HOME");
    //Error getting home directory
    if (!home) {
        fprintf(stderr, "BuildConfigPath: HOME environment variable not set\n");
        return false;
    }
    //Create path
    int written = snprintf(outPath, outSize, "%s/%s", home, relativePath);
    //If invalid path, error
    if (written < 0 || (size_t)written >= outSize) {
        fprintf(stderr, "BuildConfigPath: path too long, truncated\n");
        return false;
    }
    return true;
}

//Copy config file from source to the destination
static bool Play_CopyConfig(const char* srcPath, const char* destPath) {
    FILE* src = fopen(srcPath, "rb");
    //Error opening file
    if (!src) {
        fprintf(stderr, "CopyConfigFile: couldn't open template %s\n", srcPath);
        return false;
    }
    FILE* dest = fopen(destPath, "wb");
    //Error opening file
    if (!dest) {
        fprintf(stderr, "CopyConfigFile: couldn't open destination %s\n", destPath);
        fclose(src);
        return false;
    }
    char buffer[BUFFER_WRITE_LEN];
    size_t bytesRead;
    //Copy all the contents of the source file to the destination file
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytesRead, dest);
    }
    fclose(src);
    fclose(dest);
    return true;
}

//Find the value for a given SDL mapping key (e.g. "a", "leftx") in a mapping string
static bool Play_FindSDLMappingValue(const char* mappingStr, const char* key, char* outValue, size_t outSize) {
    char keyPrefix[32];
    snprintf(keyPrefix, sizeof(keyPrefix), ",%s:", key);
    const char* found = strstr(mappingStr, keyPrefix);
    //If key isn't found, exit
    if (!found) return false;
    found += strlen(keyPrefix);
    const char* end = strchr(found, ',');
    size_t len = end ? (size_t)(end - found) : strlen(found);
    //If length too big, shrink it
    if (len >= outSize) len = outSize - 1;
    strncpy(outValue, found, len);
    outValue[len] = '\0';
    return true;
}

//Extract the raw numeric index from a value like "b3", "a2", or "h0.1" (hat index only)
static int Play_ExtractSDLIndex(const char* value) {
    //If it finds a mapping letter, extract the number after it
    if (value[0] == 'b' || value[0] == 'a' || value[0] == 'h') {
        return atoi(value + 1);
    }
    return -1;
}

//Sort order of axes
static int Play_CompareAxisEntries(const void* a, const void* b) {
    return ((const AxisEntry*)a)->axisIndex - ((const AxisEntry*)b)->axisIndex;
}

//Sort order of buttons
static int Play_CompareButtonEntries(const void* a, const void* b) {
    return ((const ButtonEntry*)a)->buttonIndex - ((const ButtonEntry*)b)->buttonIndex;
}

//Generate the correct Flycast mappings for the specific controller
static bool Play_GenerateFlycastMapping(const char* sdlMappingStr, const char* controllerName, const char* destPath) {
    FILE* dest = fopen(destPath, "w");
    //Check for error opening destination file
    if (!dest) return false;
    char value[16];
    //Gather axes and sort entries
    AxisEntry axes[6];
    int axisCount = 0;
    const char* axisRoles[] = {"leftx", "lefty", "rightx", "righty", "lefttrigger", "righttrigger"};
    //Go throug possible axes
    for (int i = 0; i < 6; i++) {
        //Find the mapping value
        if (Play_FindSDLMappingValue(sdlMappingStr, axisRoles[i], value, sizeof(value))) {
            axes[axisCount].axisIndex = Play_ExtractSDLIndex(value);
            axes[axisCount].role = axisRoles[i];
            axisCount++;
        }
    }
    qsort(axes, axisCount, sizeof(AxisEntry), Play_CompareAxisEntries);
    //Write analog section and assign axes
    fprintf(dest, "[analog]\n");
    int bindNum = 0;
    int triggerAxes[2] = {-1, -1};
    int triggerCount = 0;
    //Go through the axes
    for (int i = 0; i < axisCount; i++) {
        //Check name and assign bindings
        if (strcmp(axes[i].role, "leftx") == 0) {
            fprintf(dest, "bind%d = %d-:btn_analog_left\n", bindNum++, axes[i].axisIndex);
            fprintf(dest, "bind%d = %d+:btn_analog_right\n", bindNum++, axes[i].axisIndex);
        }
        else if (strcmp(axes[i].role, "lefty") == 0) {
            fprintf(dest, "bind%d = %d-:btn_analog_up\n", bindNum++, axes[i].axisIndex);
            fprintf(dest, "bind%d = %d+:btn_analog_down\n", bindNum++, axes[i].axisIndex);
        }
        else if (strcmp(axes[i].role, "rightx") == 0) {
            fprintf(dest, "bind%d = %d-:axis2_left\n", bindNum++, axes[i].axisIndex);
            fprintf(dest, "bind%d = %d+:axis2_right\n", bindNum++, axes[i].axisIndex);
        }
        else if (strcmp(axes[i].role, "righty") == 0) {
            fprintf(dest, "bind%d = %d-:axis2_up\n", bindNum++, axes[i].axisIndex);
            fprintf(dest, "bind%d = %d+:axis2_down\n", bindNum++, axes[i].axisIndex);
        }
        else if (strcmp(axes[i].role, "lefttrigger") == 0) {
            fprintf(dest, "bind%d = %d+:btn_trigger_left\n", bindNum++, axes[i].axisIndex);
            triggerAxes[triggerCount++] = axes[i].axisIndex;
        }
        else if (strcmp(axes[i].role, "righttrigger") == 0) {
            fprintf(dest, "bind%d = %d+:btn_trigger_right\n", bindNum++, axes[i].axisIndex);
            triggerAxes[triggerCount++] = axes[i].axisIndex;
        }
    }
    //Sort the two trigger axis indices ascending, for the [emulator] triggers= line
    if (triggerAxes[0] > triggerAxes[1]) {
        int tmp = triggerAxes[0]; triggerAxes[0] = triggerAxes[1]; triggerAxes[1] = tmp;
    }
    //Gather button entries and sort
    ButtonEntry buttons[8];
    int buttonCount = 0;
    const char* buttonRoles[] = {"a", "b", "x", "y", "leftshoulder", "rightshoulder", "start", "guide"};
    const char* flycastNames[] = {"btn_a", "btn_b", "btn_x", "btn_y", "btn_z", "btn_c", "btn_start", "btn_menu"};
    //Go through the buttons
    for (int i = 0; i < 8; i++) {
        //Find mappings values
        if (Play_FindSDLMappingValue(sdlMappingStr, buttonRoles[i], value, sizeof(value))) {
            buttons[buttonCount].buttonIndex = Play_ExtractSDLIndex(value);
            buttons[buttonCount].flycastName = flycastNames[i];
            buttonCount++;
        }
    }
    qsort(buttons, buttonCount, sizeof(ButtonEntry), Play_CompareButtonEntries);
    //Write digital section
    fprintf(dest, "\n[digital]\n");
    bindNum = 0;
    //Scan buttons in order and assign bind numbers
    for (int i = 0; i < buttonCount; i++) {
        fprintf(dest, "bind%d = %d:%s\n", bindNum++, buttons[i].buttonIndex, buttons[i].flycastName);
    }
    //D-pad is always a fixed offset, right after the scanned digital buttons
    fprintf(dest, "bind%d = 256:btn_dpad1_up\n", bindNum++);
    fprintf(dest, "bind%d = 257:btn_dpad1_down\n", bindNum++);
    fprintf(dest, "bind%d = 258:btn_dpad1_left\n", bindNum++);
    fprintf(dest, "bind%d = 259:btn_dpad1_right\n", bindNum++);
    //Emulator section
    fprintf(dest, "\n[emulator]\n");
    fprintf(dest, "dead_zone = 10\n");
    fprintf(dest, "mapping_name = %s\n", controllerName);
    fprintf(dest, "rumble_power = 100\n");
    fprintf(dest, "saturation = 100\n");
    fprintf(dest, "triggers = %d,%d\n", triggerAxes[0], triggerAxes[1]);
    fprintf(dest, "version = 4\n");
    fclose(dest);
    return true;
}

//Apply controller mappings for Flycast
static void Play_ApplyFlycastMappings(void) {
    FILE* probe = popen("./sdl_name_probe", "r");
    //Check for error opening probe
    if (!probe) {
        fprintf(stderr, "Play_ApplyFlycastMappings: failed to run probe helper\n");
        return;
    }
    char nameLine[128];
    char mappingLine[512];
    //Get the lines
    while (fgets(nameLine, sizeof(nameLine), probe)) {
        size_t len = strlen(nameLine);
        //Strip newline from string
        if (len > 0 && nameLine[len - 1] == '\n') nameLine[len - 1] = '\0';
        //Read the mapping-string line that follows each name line
        if (!fgets(mappingLine, sizeof(mappingLine), probe)) break;
        len = strlen(mappingLine);
        //Strip newline from string
        if (len > 0 && mappingLine[len - 1] == '\n') mappingLine[len - 1] = '\0';
        //Skip controllers SDL couldn't provide a standard mapping for
        if (strcmp(mappingLine, "(no mapping string available)") == 0 ||
            strcmp(mappingLine, "(not recognized as a game controller — no mapping available)") == 0) {
            continue;
        }
        char mappingRelPath[256];
        snprintf(mappingRelPath, sizeof(mappingRelPath),
                 ".var/app/org.flycast.Flycast/config/flycast/mappings/SDL_%s.cfg", nameLine);
        char mappingDestPath[CONFIG_PATH_LEN];
        //Build config path and check for error
        if (Play_BuildConfigPath(mappingDestPath, sizeof(mappingDestPath), mappingRelPath)) {
            //Generate Flycast mappings and check for error
            if (Play_GenerateFlycastMapping(mappingLine, nameLine, mappingDestPath)) {
            } 
            else {
                fprintf(stderr, "Play_ApplyFlycastMappings: failed to generate mapping for '%s'\n", nameLine);
            }
        }
    }
    pclose(probe);
}

//Writes mappings blocks for each controller
static bool Play_WriteDolphinBlock(FILE* dest, const char* templatePath, int slot, const char* controllerName) {
    FILE* src = fopen(templatePath, "r");
    //Check for error opening source file
    if (!src) {
        fprintf(stderr, "WriteDolphinBlock: couldn't open template %s\n", templatePath);
        return false;
    }
    char line[256];
    //Go through all lines
    while (fgets(line, sizeof(line), src)) {
        char output[300];
        char* slotMarker = strstr(line, "{SLOT}");
        char* nameMarker = strstr(line, "{NAME}");
        //Replace slots with the number of the pad
        if (slotMarker) {
            snprintf(output, sizeof(output), "[GCPad%d]\n", slot);
            fputs(output, dest);
        }
        //Replace names with device names
        else if (nameMarker) {
            snprintf(output, sizeof(output), "Device = SDL/0/%s\n", controllerName);
            fputs(output, dest);
        }
        //Don't change other lines
        else {
            fputs(line, dest);
        }
    }
    fclose(src);
    return true;
}

//Apply controller mappings for Dolphin
static void Play_ApplyDolphinMappings(void) {
    FILE* probe = popen("./sdl_name_probe", "r");
    //Check for error opening probe
    if (!probe) {
        fprintf(stderr, "Play_ApplyDolphinMappings: failed to run probe helper\n");
        return;
    }
    char destPath[CONFIG_PATH_LEN];
    //Build config path and check for error
    if (!Play_BuildConfigPath(destPath, sizeof(destPath),
                              ".var/app/org.DolphinEmu.dolphin-emu/config/dolphin-emu/GCPadNew.ini")) {
        pclose(probe);
        return;
    }
    FILE* dest = fopen(destPath, "w");
    //Check for error opening destination file
    if (!dest) {
        fprintf(stderr, "Play_ApplyDolphinMappings: couldn't open destination %s\n", destPath);
        pclose(probe);
        return;
    }
    char line[128];
    int slot = 1;
    //Go through all controller slots
    while (slot <= 4 && fgets(line, sizeof(line), probe)) {
        size_t len = strlen(line);
        //Check for valid lines and write blocks
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        Play_WriteDolphinBlock(dest, "assets/emulator-configs/dolphin/GCPad_block.template", slot, line);
        slot++;
    }
    fclose(dest);
    pclose(probe);
}

//Finds the PID of a running process by name
static pid_t Play_FindProcess(const char* processName) {
    DIR* procDir = opendir("/proc");
    //Error opening directory
    if (!procDir) {
        fprintf(stderr, "FindProcessByName: couldn't open /proc: %s\n", strerror(errno));
        return -1;
    }
    struct dirent* entry;
    //Read all the entries in the directory
    while ((entry = readdir(procDir)) != NULL) {
        bool isNumeric = true;
        //Look for entires that are all numeric
        for (const char* c = entry->d_name; *c; c++) {
            //If a character isn't numeric, it's not what we want
            if (!isdigit((unsigned char)*c)) {
                isNumeric = false;
                break;
            }
        }
        //Skip non-numeric entries
        if (!isNumeric) continue;
        char commPath[COMM_PATH_LEN];
        //Create path
        snprintf(commPath, sizeof(commPath), "/proc/%s/comm", entry->d_name);
        FILE* f = fopen(commPath, "r");
        //If process stopped during this sequence
        if (!f) continue;
        char comm[COMM_LEN] = "";
        //Get name
        if (fgets(comm, sizeof(comm), f)) {
            size_t len = strlen(comm);
            //Get rid of new line
            if (len > 0 && comm[len - 1] == '\n') {
                comm[len - 1] = '\0';
            }
        }
        fclose(f);
        //If it matches the process name
        if (strcmp(comm, processName) == 0) {
            closedir(procDir);
            return (pid_t)atoi(entry->d_name);
        }
    }
    closedir(procDir);
    return -1; 
}

//Set active core variables 
static void Play_SetCurrentCore(const char* corePath) {
    g_isDoomActive = (strcmp(corePath, PATH_PRBOOM) == 0);
    g_isN64Active  = (strcmp(corePath, PATH_N64) == 0);
}

//Get whether doom core is active
bool Play_IsDoomActive(void) {
    return g_isDoomActive;
}

//Get whether N64 core is active
bool Play_IsN64Active(void) {
    return g_isN64Active;
}

//Tells whether the game uses libretro
static bool Play_IsLibRetro(const game_t* game) {
    return game->libRetro;
}

//Apply the key and pad maps
static void Play_ApplyMaps(const char* corePath) {
    //Reset maps
    for (int i = 0; i < RETRO_DEVICE_ID_JOYPAD_R3 + 1; i++) { 
        your_key_map[i] = 0; 
        your_pad_map[i] = 0; 
    }
    //Common controls
    your_key_map[RETRO_DEVICE_ID_JOYPAD_UP]     = KEY_W;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_DOWN]   = KEY_S;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_LEFT]   = KEY_A;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = KEY_D;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_START]  = KEY_ENTER;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_SELECT] = KEY_RIGHT_SHIFT;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_UP]     = GAMEPAD_BUTTON_LEFT_FACE_UP;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_DOWN]   = GAMEPAD_BUTTON_LEFT_FACE_DOWN;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_LEFT]   = GAMEPAD_BUTTON_LEFT_FACE_LEFT;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_START]  = GAMEPAD_BUTTON_MIDDLE_RIGHT;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_SELECT] = GAMEPAD_BUTTON_MIDDLE_LEFT;
    //NES and Atari Lynx controls
    if (strcmp(corePath, PATH_NES) == 0 || strcmp(corePath, PATH_LYNX) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_K;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_J;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
    }
    //Game Boy color controls
    else if (strcmp(corePath, PATH_GAMEBOY) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_K;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_J;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
    }
    //SNES, TurboGrafx-16/TurboGrafx-CD, and Neo Geo Pocket Color controls
    else if (strcmp(corePath, PATH_SNES) == 0 || strcmp(corePath, PATH_TG16) == 0 || strcmp(corePath, PATH_NGPC) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_J;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_K;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_Y] = KEY_I;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_X] = KEY_L;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_U;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_O;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y] = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_X] = GAMEPAD_BUTTON_RIGHT_FACE_UP;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
    }
    //Game Boy Advance controls
    else if (strcmp(corePath, PATH_GBA) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_J;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_K;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_U;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_O;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
    }
    //Sega Genesis, Sega Master System, and Sega CD controls
    else if (strcmp(corePath, PATH_GENESIS) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_J;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_K;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_Y] = KEY_I;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_X] = KEY_L;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_U;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_O;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y] = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_X] = GAMEPAD_BUTTON_RIGHT_FACE_UP;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
    }
    // PS1 and arcade controls
    else if (strcmp(corePath, PATH_PS1) == 0 || strcmp(corePath, PATH_ARCADE) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B]  = KEY_K;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A]  = KEY_L;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_Y]  = KEY_J;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_X]  = KEY_I;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L]  = KEY_U;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R]  = KEY_O;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L2] = KEY_Q;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R2] = KEY_E;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B]  = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A]  = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y]  = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_X]  = GAMEPAD_BUTTON_RIGHT_FACE_UP;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L]  = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R]  = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L2] = GAMEPAD_BUTTON_LEFT_TRIGGER_2;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R2] = GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
    }
    //N64 controls
    else if (strcmp(corePath, PATH_N64) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_UP]    = KEY_UP;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_DOWN]  = KEY_DOWN;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_LEFT]  = KEY_LEFT;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_RIGHT] = KEY_RIGHT;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_Y]  = KEY_U;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B]  = KEY_O;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L]  = KEY_Q;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R]  = KEY_E;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L2] = KEY_SPACE; 
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y]  = GAMEPAD_BUTTON_RIGHT_FACE_DOWN; 
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B]  = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L]  = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R]  = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L2] = GAMEPAD_BUTTON_LEFT_TRIGGER_2;
    }
    //Doom controls
    else if (strcmp(corePath, PATH_PRBOOM) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_LEFT]  = 0; 
        your_key_map[RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L]  = KEY_A;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R]  = KEY_D;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_X]  = KEY_F;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R2] = KEY_E;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L2] = KEY_Q;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A]  = KEY_SPACE;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_Y]  = KEY_LEFT_SHIFT;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_SELECT] = KEY_TAB;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_X] = GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L2] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R2] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y] = GAMEPAD_BUTTON_LEFT_THUMB;
        }
}

//Get the aspect ratio for the console
static float Play_GetConsoleAspect(const char* console) {
    //Depending on the console, return the correct aspect ratio
    if (strcmp(console, "Game Boy Color") == 0) {
       return ASPECT_GB;
    }
    else if (strcmp(console, "Game Boy Advance") == 0) {
        return ASPECT_GBA;
    }    
    else if (strcmp(console, "Sega Game Gear") == 0) {
        return ASPECT_GG;
    }
    else if (strcmp(console, "Neo Geo Pocket Color") == 0) {
        return ASPECT_NGPC;
    }
    //Default aspect ratio
    return ASPECT_OTHER;
}

//Stop the libretro game
void Play_StopLib(const game_t* game) {
    //Exit if game is not running
    if (!is_game_running) return;
    //If the game saves by battery, save before closing game
    if (game->save == BATTERY) {
        SaveBattery(game->romPath);
    }
    //Stop game audio
    StopRetroAudio();
    //Unload game
    if (core_unload_game) {
        core_unload_game();
    }
    //Release/destroy GPU resources
    TriggerContextDestroy();
    //Unload hardware render drawing texture
    if (hw_target.id != 0) {
        UnloadRenderTexture(hw_target);
        hw_target.id = 0;
    }
    //Unload main emulator drawing texture
    if (emulator_texture.id != 0) {
        UnloadTexture(emulator_texture);
        emulator_texture.id = 0;
    }
    //Close the core
    CloseRetroCore();
    //Mark that the game is no longer running
    is_game_running = false;
    //Make cursor visible
    EnableCursor();
}

//Advance libretro game
static void Play_Advance(void) {
    double step = 1.0f / core_fps;
    double frameTime = GetFrameTime();
    //Guard against stalls
    if (frameTime > MAX_FRAME_TIME) {
        frameTime = MAX_FRAME_TIME;
    }
    accumulator += frameTime;
    //Cap the accumulator
    if (accumulator > step * ACCUMULATOR_MAX_MULTIPLIER) {
        accumulator = step;
    }
    //Wait for the time on the frame to be greater than or equal to the frame target time
    while (accumulator >= step) {
        //Run the core
        if (is_game_running && core_run) {
            core_run();
        }
        accumulator -= step;
    }
    //Present the frame
    PresentFrame();
}

//Draw the game
static void Play_Draw(const game_t* game) {
    ClearBackground(BLACK);
    //Determine whether game must be rotated
    unsigned game_rotation = GetGameRotation();
    bool swapped = (game_rotation == ROTATION_90 || game_rotation == ROTATION_270);
    float texW = (float)emulator_texture.width;
    float texH = (float)emulator_texture.height;
    float targetAspect;
    //Get target aspect ratio depending on the console
    if (strcmp(game->console, "Arcade") == 0) {
        //Check if width and height are correct values
        if ((texW > 0.0f) && (texH > 0.0f)) {
            targetAspect = texW / texH;
        }
        //Fallback case
        else {
            targetAspect = ASPECT_OTHER;
        }
    }
    else {
        targetAspect = Play_GetConsoleAspect(game->console);
    }
    //Guard against bad aspects
    if (targetAspect <= 0.0f) {
        targetAspect = ASPECT_OTHER;
    }
    //If game is rotated sideways, flip the aspect ratio
    if (swapped) {
        targetAspect = 1.0f / targetAspect;
    }
    float destHeight = Var_GetMonitorHeight();
    float destWidth  = destHeight * targetAspect;
    //If the width it too much, fit the width to the screen and recompute height
    if (destWidth > Var_GetMonitorWidth()) {
        destWidth  = Var_GetMonitorWidth();
        destHeight = destWidth / targetAspect;
    }
    float rot = -(game_rotation * ROTATION_DEGREES);
    float drawW = swapped ? destHeight : destWidth;
    float drawH = swapped ? destWidth  : destHeight;
    printf("height: %f\n", drawH);
    printf("width: %f\n", drawW);
    Rectangle src  = { 0.0f, 0.0f, texW, texH };
    Rectangle dest = { Var_GetMonitorWidth() / 2.0f, Var_GetMonitorHeight() / 2.0f, drawW, drawH };
    Vector2  origin = { drawW / 2.0f, drawH / 2.0f };
    //Draw texture on the screen
    DrawTexturePro(emulator_texture, src, dest, origin, rot, WHITE);
    //Draw diagnostics (if applies)
    UI_DrawDispDiag(true);
}

//Kill external app
static void Play_KillApp(const game_t* game, int sig) {
    pid_t realPid = Play_FindProcess(game->processName);
    //If process is running, kill it safely
    if (realPid > 0) {
        kill(realPid, sig);
    } 
    else {
        kill(extAppId, sig);
    }
}

//Play initialization
void Play_Init(const game_t* game) {
    currentPlayState = PLAY_GO;
    saveTimeElapsed = 0.0f;
    resuming = false;
    resumeTimer = 0.0f;
    is_game_running = false;
    //Initialize depending on type of game
    if (Play_IsLibRetro(game)) {
        if (strcmp(game->corePath, PATH_PS1) == 0) {
    Games_SnapshotSaveFolder("assets/saves");
}
        //Reset from last game
        SetGameRotation(0);
        UnloadTexture(emulator_texture);
        //Blank texture
        Image blank = GenImageColor(BLANK_GAME_TEXT_W, BLANK_GAME_TEXT_H, BLACK);
        emulator_texture = LoadTextureFromImage(blank);
        UnloadImage(blank);
        SetTextureFilter(emulator_texture, TEXTURE_FILTER_POINT);
        //Set current core variables (for DOOM and N64)
        Play_SetCurrentCore(game->corePath);
        //Apply controls depending on the game
        Play_ApplyMaps(game->corePath);
        //Load the libretro core
        if (LoadRetroCore(game->corePath)) {
            //Load the game
            if (LoadGame(game->romPath)) {
                is_game_running = true;
                //For hardware rendered games
                if (hw_target.id == 0) {
                    hw_target = LoadRenderTexture(BLANK_GAME_TEXT_W, BLANK_GAME_TEXT_H);
                }
                SetHWFramebuffer(hw_target.id);
                TriggerContextReset();
                //If game saves through battery, load the last save
                if (game->save == BATTERY) {
                    LoadBattery(game->romPath);
                }
                core_fps = GetCoreTargetFPS();
                StartRetroAudio();
                accumulator = 0.0;
                DisableCursor();
            }
            else {
                is_game_running = false;
            }
        }
        saveTimeElapsed = 0;
        currentPlayState = PLAY_GO;
    }
    else {
        //Use correct config files depending on the application
        if (strcmp(game->corePath, PATH_GAMECUBE) == 0) {
            //Update config file
            if (Play_BuildConfigPath(dolphinIniPath, sizeof(dolphinIniPath), PATH_DOLPHIN_INI_DEST)) {
                Play_CopyConfig(PATH_DOLPHIN_INI_SRC, dolphinIniPath);
            }
            Play_ApplyDolphinMappings();
        }
        else if (strcmp(game->corePath, PATH_DS) == 0) {
            //Update config file
            if (Play_BuildConfigPath(melonDSPath, sizeof(melonDSPath), PATH_MELON_DEST)) {
                Play_CopyConfig(PATH_MELON_SRC, melonDSPath);
            }
        }
        else if (strcmp(game->corePath, PATH_SATURN) == 0) {
            char savestatesDir[CONFIG_PATH_LEN];
    if (Play_BuildConfigPath(savestatesDir, sizeof(savestatesDir),
                              ".var/app/io.github.strikerx3.ymir/data/StrikerX3/Ymir/savestates")) {
        Games_SnapshotSaveFolder(savestatesDir);
        
    }
            //Update config file
            if (Play_BuildConfigPath(saturnPath, sizeof(saturnPath), PATH_SATURN_DEST)) {
                Play_CopyConfig(PATH_SATURN_SRC, saturnPath);
            }
        }
        else if (strcmp(game->corePath, PATH_DREAMCAST) == 0) {
            //Update config file
            if (Play_BuildConfigPath(flycastEmuPath, sizeof(flycastEmuPath), PATH_FLYCAST_EMU_DEST)) {
                Play_CopyConfig(PATH_FLYCAST_EMU_SRC, flycastEmuPath);
            }
            Play_ApplyFlycastMappings();
            char flycastSaveDir[CONFIG_PATH_LEN];
    if (Play_BuildConfigPath(flycastSaveDir, sizeof(flycastSaveDir), ".var/app/org.flycast.Flycast/data/flycast")) {
        Games_SnapshotSaveFolder(flycastSaveDir);
    }
        }
        else if (strcmp(game->corePath, PATH_PSP) == 0) {
            //Update ini file
            if (Play_BuildConfigPath(ppssppIniPath, sizeof(ppssppIniPath), PATH_PPSSPP_INI_DEST)) {
                Play_CopyConfig(PATH_PPSSPP_INI_SRC, ppssppIniPath);
            }
            //Update controls file
            if (Play_BuildConfigPath(ppssppControlsPath, sizeof(ppssppControlsPath), PATH_PPSSPP_CONTROLS_DEST)) {
                Play_CopyConfig(PATH_PPSSPP_CONTROLS_SRC, ppssppControlsPath);
            }
        }
        //Create new process
        pid_t pid = fork();
        //Child process
        if (pid == 0) {
            //Correct command based on emulator
            if (strcmp(game->corePath, PATH_GAMECUBE) == 0 || strcmp(game->corePath, PATH_DREAMCAST) == 0) {
                execlp("flatpak", "flatpak", "run", game->corePath, game->romPath, (char*)NULL);
            }
            else if (strcmp(game->corePath, PATH_DS) == 0 || strcmp(game->corePath, PATH_SATURN) == 0){
                execlp("flatpak", "flatpak", "run", game->corePath, "-f", game->romPath, (char*)NULL);
            }
            else {
                execlp("flatpak", "flatpak", "run", game->corePath, "--fullscreen", game->romPath, (char*)NULL);
            }
            fprintf(stderr, "Failed to launch: %s\n", strerror(errno));
            _exit(1);
        }
        //Parent
        else if (pid > 0) {
            extAppId = pid;
        }
        //Failed
        else {
            fprintf(stderr, "Fork failed: %s\n", strerror(errno));
        }
        killEscalated = false;
        killRequestedTime = 0;
    }
}

//Play external game tick function
bool Play_TickExt(const game_t* game) {
    //Nothing is running
    if (extAppId < 0) return false;
    //If home button pressed, close the app
    if (IsKeyPressed(KEY_ESCAPE) || HOME_PRESS) {
        Play_KillApp(game, SIGTERM);
        killRequestedTime = GetTime();
    }
    int status;
    pid_t result = waitpid(extAppId, &status, WNOHANG);
    //If no longer running
    if (result == extAppId) {
        extAppId = -1;
        char savestatesDir[CONFIG_PATH_LEN];
    if (Play_BuildConfigPath(savestatesDir, sizeof(savestatesDir),
                              ".var/app/io.github.strikerx3.ymir/data/StrikerX3/Ymir/savestates")) {
        Games_DetectNewYmirSaveFolder(game, savestatesDir);
    }
    char flycastSaveDir[CONFIG_PATH_LEN];
    if (Play_BuildConfigPath(flycastSaveDir, sizeof(flycastSaveDir), ".var/app/org.flycast.Flycast/data/flycast")) {
        Games_DetectNewFlycastSaveFile(game, flycastSaveDir);
    }
        SetWindowFocused();
        Controller_SetWasPressed_Home(true);
        return false;
    }
    //If kill has been requested but not escalated yet
    if (killRequestedTime > 0 && !killEscalated) {
        //If kill was requested long enough ago, escalate the kill
        if (GetTime() - killRequestedTime >= KILL_TIME) {
            Play_KillApp(game, SIGKILL);
            killEscalated = true;
        }
    }
    return true;
}

//Play libretro game tick function
bool Play_TickLib(const game_t* game) {
    //Transition
    switch (currentPlayState) {
        //Game is running
        case PLAY_GO:
            //Pause game on Home press (or ESCAPE)
            if (IsKeyPressed(KEY_ESCAPE) || HOME_PRESS) {
                Controller_SetWasPressed_B(true);
                Controller_SetWasPressed_Home(true);
                currentPlayState = PLAY_PAUSE;
                pauseState = PLAY_PAUSE;
                PlayPause_Init();
            }
            break;

        //Game is paused
        case PLAY_PAUSE:
            //Change state of game depending on the pause menu
            if (pauseState == PLAY_RESUME) {
                resumeTimer = 0.0f;
                currentPlayState = PLAY_RESUME;
            }
            else if (pauseState == PLAY_RESTART) {
                currentPlayState = PLAY_RESTART;
            }
            else if (pauseState == PLAY_EXIT) {
                currentPlayState = PLAY_EXIT;
            }
            break;

        //Resuming game
        case PLAY_RESUME:
            //If it has been resuming long enough, stop resuming
            if (resumeTimer >= RESUME_TIME) {
                currentPlayState = PLAY_GO;
            }
            break;

        //Restarting game
        case PLAY_RESTART:
            ClearBackground(BLACK);
            Play_StopLib(game);
            Play_Init(game);
            currentPlayState = PLAY_GO;
            break;

        //Exiting game
        case PLAY_EXIT:
            break;
    }

    //Action
    switch (currentPlayState) {
        //Game is running
        case PLAY_GO:
            Play_Advance();
            Play_Draw(game);
            saveTimeElapsed += GetFrameTime();
            //If it has been long enough since last saved
            if (saveTimeElapsed >= SAVE_TIME) {
                //If game saves by battery, save correctly
                if (game->save == BATTERY) {
                    SaveBattery(game->romPath);
                }
                saveTimeElapsed = 0.0f;
            }
            break;

        //Game is paused
        case PLAY_PAUSE:
            Play_Draw(game);
            pauseState = PlayPause_Tick();
            break;

        //Resuming game
        case PLAY_RESUME:
            Play_Draw(game);
            resumeTimer += GetFrameTime();
            //Draw resuming text
            Vector2 resumeSize = MeasureTextEx(Var_GetFontRegular(), PLAY_RESUME_TXT, PLAY_RESUME_SIZE, PLAY_RESUME_SPACE);
            Vector2 resume = {PLAY_RESUME_X, PLAY_RESUME_Y};
            DrawRectangle(PLAY_RESUME_RECT_X, PLAY_RESUME_RECT_Y, PLAY_RESUME_RECT_W, PLAY_RESUME_RECT_H, BLACK);
            DrawRectangleLines(PLAY_RESUME_RECT_X, PLAY_RESUME_RECT_Y, PLAY_RESUME_RECT_W, PLAY_RESUME_RECT_H, WHITE);
            DrawTextEx(Var_GetFontRegular(), PLAY_RESUME_TXT, resume, PLAY_RESUME_SIZE, PLAY_RESUME_SPACE, WHITE);
            break;
        
        //Restarting game
        case PLAY_RESTART:
            break;

        //Exiting game
        case PLAY_EXIT:
            Games_DetectNewPS1SaveFile(game, "assets/saves");
            Play_StopLib(game);
            return false;
    }
    return true;
}
