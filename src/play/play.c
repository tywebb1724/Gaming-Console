#include "play.h"

#include "config.h"
#include "emulators/libretro/retro_bridge.h"
#include "playpause.h"
#include "controller_config.h"
#include "raylib.h"
#include "var.h"

//Monitor width and height
static int Var_GetMonitorWidth();
static int Var_GetMonitorHeight();
//Key and pad maps
static int your_key_map[RETRO_DEVICE_ID_JOYPAD_MASK];
static int your_pad_map[RETRO_DEVICE_ID_JOYPAD_MASK];
//Textures for drawing the games
static Texture2D emulator_texture;
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

/*
//Detect the controllers
static void Play_DetectController() {
    g_controllerIndex = -1;
    //Go through all available gamepad slots
    for (int i = 0; i < 8; i++) {
        //If there is a gamepad connected
        if (IsGamepadAvailable(i)) {
            //Get name of gamepad
            const char *name = GetGamepadName(i);
            //If it is a valid Xbox controller, save the index of the gamepad
            if (name && (strstr(name, "GameSir") || strstr(name, "Xbox") ||
                         strstr(name, "X-Box")   || strstr(name, "Microsoft") ||
                         strstr(name, "Controller"))) {
                g_controllerIndex = i;
                return;
            }
        }
    }
}
*/

//Tells whether the game uses libretro
static bool Play_IsLibRetro(game_t game) {
    //Check if the game is uses libretro or not
    if (game.libRetro == true) {
        return true;
    }
    else {
        return false;
    }
}

//Apply the key and pad maps
static void Play_ApplyMaps(char* corePath) {
    for (int i = 0; i < 32; i++) { your_key_map[i] = 0; your_pad_map[i] = 0; }

        //Common controls
        your_key_map[RETRO_DEVICE_ID_JOYPAD_UP]     = KEY_UP;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_DOWN]   = KEY_DOWN;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_LEFT]   = KEY_LEFT;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = KEY_RIGHT;
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
            your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_X;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
        }
        //Game Boy color controls
        else if (strcmp(corePath, PATH_GAMEBOY) == 0) {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_X;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
        }
        //SNES, TurboGrafx-16/TurboGrafx-CD, and Neo Geo Pocket Color controls
        else if (strcmp(corePath, PATH_SNES) == 0 || strcmp(corePath, PATH_TG16) == 0 || strcmp(corePath, PATH_NGPC) == 0) {
            your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_X;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_Y] = KEY_A;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_X] = KEY_S;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_Q;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_W;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y] = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_X] = GAMEPAD_BUTTON_RIGHT_FACE_UP;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
        }
        //Game Boy Advance controls
        else if (strcmp(corePath, PATH_GBA) == 0) {
            your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_X;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_Q;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_W;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
        }
        //Sega Genesis, Sega Master System, and Sega CD controls
        else if (strcmp(corePath, PATH_GENESIS) == 0) {
            your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_X;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_Y] = KEY_A;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_X] = KEY_S;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_Q;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_W;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y] = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_X] = GAMEPAD_BUTTON_RIGHT_FACE_UP;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
        }
        // PS1 and arcade controls
        else if (strcmp(corePath, PATH_PS1) == 0 || strcmp(corePath, PATH_ARCADE) == 0) {
            your_key_map[RETRO_DEVICE_ID_JOYPAD_B]  = KEY_Z;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_A]  = KEY_X;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_Y]  = KEY_A;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_X]  = KEY_S;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_L]  = KEY_Q;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_R]  = KEY_W;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_L2] = KEY_E;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_R2] = KEY_R;
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
            your_key_map[RETRO_DEVICE_ID_JOYPAD_Y]  = KEY_X;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_B]  = KEY_Z;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_L]  = KEY_Q;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_R]  = KEY_W;
            your_key_map[RETRO_DEVICE_ID_JOYPAD_L2] = KEY_SPACE;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y]  = GAMEPAD_BUTTON_RIGHT_FACE_DOWN; 
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_B]  = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_L]  = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_R]  = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_L2] = GAMEPAD_BUTTON_LEFT_TRIGGER_2;
        }
        //Doom controls
        else if (strcmp(corePath, PATH_PRBOOM) == 0) {
            your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;          // Fire
            your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_SPACE;      // Use/Open
            your_key_map[RETRO_DEVICE_ID_JOYPAD_Y] = KEY_LEFT_SHIFT; // Run
            your_key_map[RETRO_DEVICE_ID_JOYPAD_X] = KEY_X;          // extra
            your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_Q;          // prev weapon
            your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_W;          // next weapon
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_UP;  // Fire = Xbox A
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; // Use = Xbox B
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;    // Run = Xbox Y
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_X] = GAMEPAD_BUTTON_RIGHT_TRIGGER_2;  // Xbox X
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;   // weapon cycle
            your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;  // weapon cycle
        }
}

//Get the aspect ratio for the console
static float Play_GetConsoleAspect(const char* console) {
    //Depending on the console, return the correct aspect ratio
    if (strcmp(console, "Game Boy") == 0) {
        return 10.0f / 9.0f;
    }
    else if (strcmp(console, "Game Boy Color") == 0) {
       return 10.0f / 9.0f;
    }
    else if (strcmp(console, "Game Boy Advance") == 0) {
        return 3.0f / 2.0f;
    }    
    else if (strcmp(console, "Game Gear") == 0) {
        return 6.0f / 5.0f;
    }
    else if (strcmp(console, "Neo Geo Pocket Color") == 0) {
        return 20.0f / 19.0f;
    }
    //Default aspect ratio
    return 4.0f / 3.0f;
}

//Stop the game
void Play_Stop(game_t game) {
    //If a game is currently running
    if (is_game_running) {
        //If the game saves by battery, save before closing game
        if (game.save == BATTERY) {
            SaveBattery(game.romPath);
        }
        //Stop game audio
        StopRetroAudio();
        //Unload game
        if (core_unload_game) {
            core_unload_game();
        }
        //Release/destroy GPU resources
        TriggerContextDestroy();
        //Close the core
        CloseRetroCore();
        //Mark that the game is no longer running
        is_game_running = false;
    }
}

//Advance the game
static void Play_Advance() {
    //How long one frame should take
    double step = 1.0 / core_fps;
    //How long last frame took
    double frameTime = GetFrameTime();
    //Guard against stalls
    if (frameTime > 0.25) {
        frameTime = 0.25;
    }
    //Keep track how long this frame has been up
    accumulator += frameTime;
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
static void Play_Draw(game_t game) {
    ClearBackground(BLACK);
    //Determine whether game must be rotated
    unsigned game_rotation = GetGameRotation();
    //If game is rotated sideways (rotated 90 or 270 degrees)
    bool swapped = (game_rotation == 1 || game_rotation == 3);
    //Width and height of the texture
    float texW = (float)emulator_texture.width;
    float texH = (float)emulator_texture.height;
    //Define target aspect ration
    float targetAspect;
    //Get target aspect ratio depending on the console
    if (strcmp(game.console, "Arcade") == 0) {
        targetAspect = texW / texH;
    }
    else {
        targetAspect = Play_GetConsoleAspect(game.console);
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
    MainMenu_DrawDiagnostics();
}

//Play initialization
void Play_Init(game_t game) {
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
        Image blank = GenImageColor(640, 480, BLACK);
        emulator_texture = LoadTextureFromImage(blank);
        UnloadImage(blank);
        //Keep pixels sharp
        SetTextureFilter(emulator_texture, TEXTURE_FILTER_POINT);
        //Apply controls depending on the game
        Play_ApplyMaps(game.corePath);
        //Load the libretro core
        if (LoadRetroCore(game.corePath)) {
            //Load the game
            if (LoadGame(game.romPath)) {
                //Mark that the game is running
                is_game_running = true;
                //For hardware rendered games
                if (hw_target.id == 0) {
                    hw_target = LoadRenderTexture(640, 480);
                }
                SetHWFramebuffer(hw_target.id);
                //Reinitialize GPU resources
                TriggerContextReset();
                //If game saves through battery, load the last save
                if (game.save == BATTERY) {
                    LoadBattery(game.romPath);
                }
                //Get the FPS for the game
                core_fps = GetCoreTargetFPS();
                //Start the audio
                StartRetroAudio();
                //Reset frame timing
                accumulator = 0.0;
            }
            else {
                //printf("CRITICAL: LoadGame() returned false. Aborting launch.\n");
                is_game_running = false; // Prevent the run loop
                // Optionally: unload core here
            }
        }
        //Reset time since last saved game
        saveTimeElapsed = 0;
        //Initialize state of the game
        currentPlayState = GAME_GO;
    }
    //If the game is through an external application
    else {
        //If it is a DS or Saturn game, start the script for exiting the applcication when the right button is pressed
        if (strcmp(game.corePath, PATH_DS) == 0) {
            system("flatpak run io.github.antimicrox.antimicrox --profile /home/tywebb1724/Desktop/Gaming-Console/assets/antimicro/micro_ds.gamecontroller.amgp --hidden &");
            usleep(4000000);
        }
        else if (strcmp(game.corePath, PATH_SATURN) == 0) {
            system("flatpak run io.github.antimicrox.antimicrox --profile /home/tywebb1724/Desktop/Gaming-Console/assets/antimicro/micro_saturn.gamecontroller.amgp --hidden &");
            usleep(4000000);
        }
        //Run the correct command depending on the console
        char command[1024];
        snprintf(command, sizeof(command), "flatpak run %s \"%s\"", game.corePath, game.romPath);
        system(command);
        //If it is a DS or Saturn game, kill the script
        if (strcmp(game.corePath, PATH_DS) == 0) {
            system("flatpak kill io.github.antimicrox.antimicrox");
        }
        else if (strcmp(game.corePath, PATH_SATURN) == 0) {
            system("flatpak kill io.github.antimicrox.antimicrox");
        }
        //Focus the menu window again
        SetWindowFocused();
    }
}

//Play game tick function
bool Play_Tick(game_t game) {

    //Transition
    switch (currentPlayState) {
        case PLAY_GO:
            //Pause game on Home press (or P)
            if (IsKeyPressed(KEY_P) || HOME_PRESS) {
                currentPlayState = PLAY_PAUSE;
                pauseState = PLAY_PAUSE;
                PlayPause_Init();
            }
            break;

        case PLAY_PAUSE:
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

        case PLAY_RESUME:
            //If it has been resuming long enough, stop resuming
            if (resumeTimer >= RESUME_TIME) {
                currentPlayState = PLAY_GO;
            }
            break;

        case PLAY_RESTART:
            ClearBackground(BLACK);
            Play_Stop(game);
            Play_Init(game);
            currentPlayState = PLAY_GO;
            break;

        case PLAY_EXIT:
            break;
    }

    //Action
    switch (currentPlayState) {
        case PLAY_GO:
            //Advance the game
            Play_Advance();
            //Draw the game
            Play_Draw(game);
            //See how long since game has last saved
            saveTimeElapsed += GetFrameTime();
            //If it has been a minute since last saved
            if (saveTimeElapsed >= 60.0f) {
                //If game saves by battery, save correctly
                if (game.save == BATTERY) {
                    SaveBattery(game.romPath);
                }
                //Reset save timer
                saveTimeElapsed = 0.0f;
            }
            break;

        case PLAY_PAUSE:
            //Draw the game
            Play_Draw(game);
            //Draw pause menu
            pauseState = PlayPause_Tick();
            break;

        case PLAY_RESUME:
            //Draw the game
            Play_Draw(game);
            //See how long since started resuming
            resumeTimer += GetFrameTime();
            //Draw resuming text
            Vector2 resumeSize = MeasureTextEx(Var_GetFontRegular(), RESUME_TXT, RESUME_SIZE, RESUME_SPACE);
            Vector2 resume = {RESUME_X, RESUME_Y};
            DrawRectangle(RESUME_RECT_X, RESUME_RECT_Y, RESUME_RECT_W, RESUME_RECT_H, BLACK);
            DrawRectangleLines(RESUME_RECT_X, RESUME_RECT_Y, RESUME_RECT_W, RESUME_RECT_H, WHITE);
            DrawTextEx(Var_GetFontRegular(), RESUME_TXT, resume, RESUME_SIZE, RESUME_SPACE, WHITE);
            break;
        
        case PLAY_RESTART:
            break;

        case PLAY_EXIT:
            return false;
    }

    return true;
}





