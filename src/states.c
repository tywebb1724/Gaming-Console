#include "states.h"
#include "raylib.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "categories.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

//Time the boot up screen should display
#define BOOT_TIME 3.0f
//State of the console
static ConsoleState currentConsoleState;
static ConsoleState changeConsoleState;
//Timer to keep track of the time on the boot screen
static float bootTimer;
//Variable to keep track of the amount of games loaded
int gamesLoaded;
//Variable to show whether all the game covers have been loaded
bool allLoaded;
//Thread for loaded images
pthread_t loadThread;
//Spider logo texture
Texture2D spiderLogo;

Texture2D background_Blue;
Texture2D background_Red;
Texture2D background_Green;
Texture2D background_Yellow;
Texture2D currentBackground;

bool exitApp;

double d0 = 0;
    double d1 = 0;
    double d2 = 0;
    double d3 = 0;
    double d4 = 0;


const Color NametoColor(char *c) {
    if (strcmp(c, "BLUE") == 0) {
        return BLUE;
    }
    else if (strcmp(c, "RED") == 0) {
        return RED;
    }
    else if (strcmp(c, "BLACK") == 0) {
        return BLACK;
    }
    else if (strcmp(c, "WHITE") == 0) {
        return WHITE;
    }
    else if (strcmp(c, "GREEN") == 0) {
        return GREEN;
    }
    else if (strcmp(c, "YELLOW") == 0) {
        return YELLOW;
    }
    return BLUE;   // fallback if it matches nothing
}

const Texture2D NametoBackground(char *c) {
    if (strcmp(c, "BLUE") == 0) {
        return background_Blue;
    }
    else if (strcmp(c, "RED") == 0) {
        return background_Red;
    }
    else if (strcmp(c, "GREEN") == 0) {
        return background_Green;
    }
    else if (strcmp(c, "YELLOW") == 0) {
        return background_Yellow;
    }
    return background_Blue;
}

void State_ClearGameData(Game game) {
    if (game.save == BATTERY) {
        char save_path[512];
        snprintf(save_path, sizeof(save_path), "%s.srm", game.romPath);
        if (remove(save_path) == 0) {
            printf("Cleared save data: %s\n", save_path);
        } 
        else {
            // remove() fails if the file doesn't exist — that's not necessarily an error,
            // it just means there was no save to clear
            printf("No save data to clear (or delete failed): %s\n", save_path);
        }
    }
    else if (game.save == EXTERNAL) {
        if (strcmp(game.console, "Sony PlayStation") == 0) {
            char save_path[512];
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/Desktop/Gaming-Console/assets/saves/%s.mcd", game.serial);
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                // remove() fails if the file doesn't exist — that's not necessarily an error,
                // it just means there was no save to clear
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
        else if (strcmp(game.console, "Sega CD") == 0) {
            char save_path[512];
            char path[256];
            snprintf(path, sizeof(path), "%s", game.romPath);
            const char* filename = strrchr(path, '/');
            filename = filename ? filename + 1 : path; 
            char* dot = strrchr(filename, '.');   // find the LAST '.' in the copied name
            if (dot) {
                *dot = '\0';                 // terminate the string there, removing extension
            }
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/Desktop/Gaming-Console/assets/saves/%s.brm", filename);
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                // remove() fails if the file doesn't exist — that's not necessarily an error,
                // it just means there was no save to clear
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
        else if (strcmp(game.console, "Sony PlayStation Portable") == 0) {
            char command[1024] = "";
            snprintf(command, sizeof(command), "rm -rf \"/home/tywebb1724/.var/app/org.ppsspp.PPSSPP/config/ppsspp/PSP/SAVEDATA/%s\"", game.serial);
            int result = system(command);
            if (result == 0) {
                printf("Cleared PSP save data for %s\n", game.serial);
            } 
            else {
                printf("Failed to clear PSP save (or none existed)\n");
            }
        }
        else if (strcmp(game.console, "Sega Saturn") == 0) {
            char command[1024] = "";
            snprintf(command, sizeof(command), "rm -rf \"/home/tywebb1724/.var/app/io.github.strikerx3.ymir/data/StrikerX3/Ymir/savestates/%s\"", game.serial);
            int result = system(command);
            if (result == 0) {
                printf("Cleared Saturn save data for %s\n", game.serial);
            } 
            else {
                printf("Failed to clear Saturn save (or none existed)\n");
            }
        }
        else if (strcmp(game.console, "Nintendo DS") == 0) {
            char save_path[512];
            char path[256];
            snprintf(path, sizeof(path), "%s", game.romPath);
            const char* filename = strrchr(path, '/');
            filename = filename ? filename + 1 : path; 
            char* dot = strrchr(filename, '.');   // find the LAST '.' in the copied name
            if (dot) {
                *dot = '\0';                 // terminate the string there, removing extension
            }
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/ds/%s.sav", filename);
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                // remove() fails if the file doesn't exist — that's not necessarily an error,
                // it just means there was no save to clear
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
        else if (strcmp(game.console, "Nintendo GameCube") == 0) {
            char save_path[512];
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/.var/app/org.DolphinEmu.dolphin-emu/data/dolphin-emu/GC/USA/Card A/%s.gci", game.serial);
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                // remove() fails if the file doesn't exist — that's not necessarily an error,
                // it just means there was no save to clear
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
        
    }
}

//Initialize the states
void State_Init() {
    //Console starts in boot state
    currentConsoleState = STATE_BOOT;
    //Boot timer starts at 0
    bootTimer = 0.0f;
    //Initially no games loaded
    gamesLoaded = 0;
    //Initialize the alphas
    alphaCategories_Out = 1.0f;
    alphaCategories_In = 0.0f;
    //Reset the coordinates for the games
    UI_ResetDisplayCoords_Games();
    //Create thread for loading images
    pthread_create(&loadThread, NULL, Games_LoadImages, NULL);
    //Load the logo textures
    spiderLogo = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/LogoBlack.png");
    background_Blue = LoadTexture("./assets/covers/logo/BlueBackground.png");
    background_Red= LoadTexture("./assets/covers/logo/RedBackground.png");
    background_Green = LoadTexture("./assets/covers/logo/GreenBackground.png");
    background_Yellow = LoadTexture("./assets/covers/logo/YellowBackground.png");
    //Start in a static position
    scrollGames = SCROLL_NO;
    scrollCategories = SCROLL_NO;
    //All textures not loaded
    allLoaded = false;
    exitApp = false;
    brightness = 0;
    brightCircleX = BRIGHTNESS_LINE_X_END;
    currentTheme = THEME_1;

    char color1[10] = "", color2[10] = "", color3[10] = "", bright[32] = "", circX[32] = "", diag[5] = "";
    FILE* f = fopen("/home/tywebb1724/Desktop/Gaming-Console/assets/system/ui.txt", "r");
    if (f) {

        if (fgets(color1, sizeof(color1), f)) {
            color1[strcspn(color1, "\n")] = '\0';
            themeColor1 = NametoColor(color1);
            currentBackground = NametoBackground(color1);
        }
        else {
            themeColor1 = BLUE;
            currentBackground = background_Blue;
        }
        if (fgets(color2, sizeof(color2), f)) {
            color2[strcspn(color2, "\n")] = '\0';
            themeColor2 = NametoColor(color2);
        }
        else {
            themeColor2 = BLACK;
        }
        if (fgets(color3, sizeof(color3), f)) {
            color3[strcspn(color3, "\n")] = '\0';
            themeColor3 = NametoColor(color3);
        }
        else {
            themeColor3 = WHITE;
        }
        if (fgets(bright, sizeof(bright), f)) {
            bright[strcspn(bright, "\n")] = '\0';
            brightness = atof(bright);

            if (fgets(circX, sizeof(circX), f)) { 
                circX[strcspn(circX, "\n")] = '\0';
                brightCircleX = atof(circX);
            }
            else {
                brightness = MAX_BRIGHTNESS;
                brightCircleX = BRIGHTNESS_CIRCLE_X;
            }
        }
        else {
            brightness = MAX_BRIGHTNESS;
            brightCircleX = BRIGHTNESS_CIRCLE_X;
        }
        if (fgets(diag, sizeof(diag), f)) {
            diag[strcspn(diag, "\n")] = '\0';
            displayDiag = atoi(diag);
        }
        else {
            displayDiag = false;
        }
        fclose(f);
    }
    char *mappings = LoadFileText("/home/tywebb1724/Desktop/Gaming-Console/assets/system/gamecontrollerdb.txt");  // raylib's file loader
if (mappings != NULL) {
    int count = SetGamepadMappings(mappings);
    UnloadFileText(mappings);
}
    InitAudioDevice();
}

//Update states and variabels and draw the correct screen
void State_UpdateAndDraw() {
    //If all games aren't dont loading
    if (!allLoaded) {
        //Searc through all the games
        for (int i = 0; i < GAMES_LEN; i++) {
            //If iamge is loaded but texture isn't uplaoded yet
            if (isLoaded[i] && !isTextureUploaded[i]) {
                //Load texture and unload image
                gameLibrary[i].cover = LoadTextureFromImage(LoadedImages[i]);
                UnloadImage(LoadedImages[i]);
                isTextureUploaded[i] = true;
                //If on the last game, all games are loaded
                if (i == GAMES_LEN - 1) {
                    allLoaded = true;
                }
            }
        }
    }

    //Transition
    switch(currentConsoleState) {
        
        //Console boots up
        case STATE_BOOT:
            //If the boot up time has passed, go to the main menu
            if (bootTimer >= BOOT_TIME) {
                currentConsoleState = STATE_MAIN_MENU;
            }
            break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
            if (changeConsoleState == STATE_APP_LAUNCHER) {
                currentConsoleState = STATE_APP_LAUNCHER;
                Games_LaunchInit(*gamesDisplayed[3]);
                SetTargetFPS(0);
            }
            else if (changeConsoleState == STATE_LIST) {
                currentConsoleState = STATE_LIST;
            }
            else if (changeConsoleState == STATE_VIEW_DIAG) {
                currentConsoleState = STATE_VIEW_DIAG;
            }
            break;

        case STATE_APP_LAUNCHER:
            if (gamesDisplayed[3]->libRetro == true) {
                if (exitApp) {
                    currentConsoleState = STATE_MAIN_MENU;
                    exitApp = false;
                    SetTargetFPS(60);
                }
            }    
            else {
                currentConsoleState = STATE_MAIN_MENU;
                SetTargetFPS(60);
            }
            break;

        case STATE_LIST:
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                //mouseWasPressed = true;
                currentConsoleState = STATE_MAIN_MENU;
            }
            break;

        case STATE_VIEW_DIAG:
            if (changeConsoleState == STATE_LIST) {
                currentConsoleState = STATE_LIST;
            }
            else if (changeConsoleState == STATE_MAIN_MENU) {
                currentConsoleState = STATE_MAIN_MENU;
            }
            break;

    }

    //Action
    switch(currentConsoleState) {

        //Console boots up
        case STATE_BOOT:
            //Draw the boot up screen
            UI_DrawBootScreen();
            //Increment boot timer
            bootTimer += GetFrameTime();
            DrawRectangle(0, 0, monitorWidth, monitorHeight, (Color){ 0, 0, 0, brightness });
            break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
            if (IsKeyPressed(KEY_M)) {
                State_ClearGameData(*gamesDisplayed[3]);
            }
            //Draw UI for the menu
            changeConsoleState = MainMenu_Tick(currentConsoleState);
            DrawRectangle(0, 0, monitorWidth, monitorHeight, (Color){ 0, 0, 0, brightness });
            break;

        case STATE_APP_LAUNCHER:
            if (gamesDisplayed[3]->libRetro == true) {
                exitApp = Games_LaunchRefresh(*gamesDisplayed[3]);
                DrawRectangle(0, 0, monitorWidth, monitorHeight, (Color){ 0, 0, 0, brightness });
            }    
            break;

        case STATE_LIST:
            DrawRectangle(0, 0, monitorWidth, monitorHeight, (Color){ 0, 0, 0, brightness });
            break;

        case STATE_VIEW_DIAG:
            changeConsoleState = UI_DrawDiagnostics(currentConsoleState);
            UI_DrawOptionsBtn();
            DrawRectangle(0, 0, monitorWidth, monitorHeight, (Color){ 0, 0, 0, brightness });
            break;
   }
}