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
        }
        else if (strcmp(game->corePath, PATH_DS) == 0) {
            //Update config file
            if (Play_BuildConfigPath(melonDSPath, sizeof(melonDSPath), PATH_MELON_DEST)) {
                Play_CopyConfig(PATH_MELON_SRC, melonDSPath);
            }
        }
        else if (strcmp(game->corePath, PATH_SATURN) == 0) {
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
            Play_StopLib(game);
            return false;
    }
    return true;
}
