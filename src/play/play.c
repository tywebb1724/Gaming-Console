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
//Current state of the game after being launce
static PlayState currentPlayState;
static PlayState pauseState;
//Time between saves
static float saveTimeElapsed;
//Variables for resuming game after being pasued
static bool resuming;
static float resumeTimer;
//Whether the game is running or not
static bool is_game_running;

//Used for hardware rendering
static RenderTexture2D hw_target = {0}; 
//Whether these cores are active
static bool g_isDoomActive = false;
static bool g_isN64Active  = false;

static pid_t extAppId = -1;
static float killRequestedTime;
static bool killEscalated;

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

// Finds the PID of a running process by name (matches /proc/<pid>/comm).
// Returns the PID if found, or -1 if not found / on error.
static pid_t Play_FindProcess(const char* processName) {
    DIR* procDir = opendir("/proc");
    if (!procDir) {
        fprintf(stderr, "FindProcessByName: couldn't open /proc: %s\n", strerror(errno));
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(procDir)) != NULL) {
        // /proc entries for processes are directories named after their PID (all digits)
        bool isNumeric = true;
        for (const char* c = entry->d_name; *c; c++) {
            if (!isdigit((unsigned char)*c)) {
                isNumeric = false;
                break;
            }
        }
        if (!isNumeric) continue;   // skip non-PID entries (self, net, cpuinfo, etc.)

        char commPath[300];
        snprintf(commPath, sizeof(commPath), "/proc/%s/comm", entry->d_name);

        FILE* f = fopen(commPath, "r");
        if (!f) continue;   // process may have exited between readdir and fopen — skip

        char comm[256] = "";
        if (fgets(comm, sizeof(comm), f)) {
            // Strip the trailing newline /proc/*/comm always includes
            size_t len = strlen(comm);
            if (len > 0 && comm[len - 1] == '\n') {
                comm[len - 1] = '\0';
            }
        }
        fclose(f);

        if (strcmp(comm, processName) == 0) {
            closedir(procDir);
            return (pid_t)atoi(entry->d_name);
        }
    }

    closedir(procDir);
    return -1;   // not found
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
    //Check if the game is uses libretro or not
    if (game->libRetro == true) {
        return true;
    }
    else {
        return false;
    }
}

//Apply the key and pad maps
static void Play_ApplyMaps(const char* corePath) {
    //Reset key maps
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
    //If a game is currently running
    if (is_game_running) {
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
        //Make cursore visible
        EnableCursor();
    }
}

//Advance libretro game
static void Play_Advance() {
    //How long one frame should take
    double step = 1.0f / core_fps;
    //How long last frame took
    double frameTime = GetFrameTime();
    //Guard against stalls
    if (frameTime > MAX_FRAME_TIME) {
        frameTime = MAX_FRAME_TIME;
    }
    //Keep track how long this frame has been up
    accumulator += frameTime;
    //Cap the accumulator
    if (accumulator > step * 2.0) {
        accumulator = step;
    }
        //Wait for the time on the frame to be greater than or equal to the frame target time
    while (accumulator >= step) {
        //Run the core
        if (is_game_running && core_run) {
            core_run();
        }
        //Subtract the target time for each frame
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
    //If game is rotated sideways (rotated 90 or 270 degrees)
    bool swapped = (game_rotation == ROTATION_90 || game_rotation == ROTATION_270);
    //Width and height of the texture
    float texW = (float)emulator_texture.width;
    float texH = (float)emulator_texture.height;
    //Define target aspect ration
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
    //Fit image to the screen
    float destHeight = Var_GetMonitorHeight();
    float destWidth  = destHeight * targetAspect;
    //If the width it too much, fit the width to the screen and recompute height
    if (destWidth > Var_GetMonitorWidth()) {
        destWidth  = Var_GetMonitorWidth();
        destHeight = destWidth / targetAspect;
    }
    //Get how much the game needs to be rotated
    float rot = -(game_rotation * 90.0f);
    //Height and width to drawing, depending on rotation
    float drawW = swapped ? destHeight : destWidth;
    float drawH = swapped ? destWidth  : destHeight;
    //Source, destination, and origin rectangles
    Rectangle src  = { 0.0f, 0.0f, texW, texH };
    Rectangle dest = { Var_GetMonitorWidth() / 2.0f, Var_GetMonitorHeight() / 2.0f, drawW, drawH };
    Vector2  origin = { drawW / 2.0f, drawH / 2.0f };
    //Draw texture on the screen
    DrawTexturePro(emulator_texture, src, dest, origin, rot, WHITE);
    //Draw diganostics (if applys)
    UI_DrawDispDiag(true);
}

//Play initialization
void Play_Init(const game_t* game) {
    //Start by running the game
    currentPlayState = PLAY_GO;
    //No time since last saved
    saveTimeElapsed = 0.0f;
    //Not resuming from pause menu
    resuming = false;
    //Resume timer set at 0
    resumeTimer = 0.0f;
    //Not running the game yet
    is_game_running = false;
    //If the game is libretro
    if (Play_IsLibRetro(game)) {
        //Reset previous rotation settings
        SetGameRotation(0);
        //Unload the previous texture from the last emulator ran
        UnloadTexture(emulator_texture);
        //Generate blank texture for game to render on
        Image blank = GenImageColor(BLANK_GAME_TEXT_W, BLANK_GAME_TEXT_H, BLACK);
        emulator_texture = LoadTextureFromImage(blank);
        UnloadImage(blank);
        //Keep pixels sharp
        SetTextureFilter(emulator_texture, TEXTURE_FILTER_POINT);
        //Set current core variables
        Play_SetCurrentCore(game->corePath);
        //Apply controls depending on the game
        Play_ApplyMaps(game->corePath);
        //Load the libretro core
        if (LoadRetroCore(game->corePath)) {
            //Load the game
            if (LoadGame(game->romPath)) {
                //Mark that the game is running
                is_game_running = true;
                //For hardware rendered games
                if (hw_target.id == 0) {
                    hw_target = LoadRenderTexture(BLANK_GAME_TEXT_W, BLANK_GAME_TEXT_H);
                }
                SetHWFramebuffer(hw_target.id);
                //Reinitialize GPU resources
                TriggerContextReset();
                //If game saves through battery, load the last save
                if (game->save == BATTERY) {
                    LoadBattery(game->romPath);
                }
                //Get the FPS for the game
                core_fps = GetCoreTargetFPS();
                //Start the audio
                StartRetroAudio();
                //Reset frame timing
                accumulator = 0.0;
                DisableCursor();
            }
            else {
                is_game_running = false;
            }
        }
        //Reset time since last saved game
        saveTimeElapsed = 0;
        //Initialize state of the game
        currentPlayState = PLAY_GO;
    }
    //If the game is through an external application
    else {
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
        pid_t realPid = Play_FindProcess(game->processName);   // see note below
        if (realPid > 0) {
            kill(realPid, SIGTERM);
        } else {
            // Fall back to the fork()'d PID in case it IS the real process
            // (e.g. an emulator launched without Flatpak wrapping it)
            kill(extAppId, SIGTERM);
        }
        killRequestedTime = GetTime();
    }

    //Check status of the result (still watches the ORIGINAL fork()'d child,
    //since that's the one this process actually owns and can wait() on)
    int status;
    pid_t result = waitpid(extAppId, &status, WNOHANG);
    //If no longer running
    if (result == extAppId) {
        extAppId = -1;
        //Focus the menu window again
        SetWindowFocused();
        Controller_SetWasPressed_Home(true);
        return false;
    }

    //If kill has been requested but not escalated yet
    if (killRequestedTime > 0 && !killEscalated) {
        //If kill was requested long enough ago, escalate the kill
        if (GetTime() - killRequestedTime >= KILL_TIME) {
            pid_t realPid = Play_FindProcess(game->processName);
            if (realPid > 0) {
                kill(realPid, SIGKILL);
            } else {
                kill(extAppId, SIGKILL);
            }
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
            //Advance the game
            Play_Advance();
            //Draw the game
            Play_Draw(game);
            //See how long since game has last saved
            saveTimeElapsed += GetFrameTime();
            //If it has been a minute since last saved
            if (saveTimeElapsed >= SAVE_TIME) {
                //If game saves by battery, save correctly
                if (game->save == BATTERY) {
                    SaveBattery(game->romPath);
                }
                //Reset save timer
                saveTimeElapsed = 0.0f;
            }
            break;

        //Game is paused
        case PLAY_PAUSE:
            //Draw the game
            Play_Draw(game);
            //Draw pause menu
            pauseState = PlayPause_Tick();
            break;

        //Resuming game
        case PLAY_RESUME:
            //Draw the game
            Play_Draw(game);
            //See how long since started resuming
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
