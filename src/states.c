#include "states.h"
#include "raylib.h"
#include "ui/ui.h"
#include "games.h"
#include "ui/ui_config.h"
#include "categories.h"
#include "play/play.h"
#include <pthread.h>
#include "var.h"
#include "config.h"
#include "ui/diagnostics.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//State of the console
static ConsoleState currentConsoleState;
static ConsoleState newConsoleState;
//Timer to keep track of the time on the boot screen
static float bootTimer;
//Variable to keep track of the amount of games loaded
static int gamesLoaded;
//Variable to show whether all the game covers have been loaded
static bool allLoaded;
//Thread for loaded images
static pthread_t loadThread;
//Variable to indicate if game is running
static bool is_game_running;
//Timer to track app launch
static float launchTimer;
//Length of longest text line in the select box
static float maxLen;

//All textures start not uploaded yet
static bool isTextureUploaded[MAX_GAMES] = { false };


//Extract a color from the color name
static Color States_NametoColor(char *c) {
    //Return the corresponding color for each name
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
    //Fallback case
    return BLUE;
}

//Load all game images (if not loaded already)
void States_LoadGameImages(void) {
    //Ignore if all images are loaded
    if (allLoaded) {
        return;
    }
    bool pending = false;
    //Search through all the games
    for (int i = 0; i < GAMES_LEN; i++) {
        //If texture already loaded, go to next one
        if (isTextureUploaded[i]) {
            continue;
        }
        //If image is loaded
        if (Games_GetIsLoaded(i)) {
            //Load texture and unload image
            game_t* game = Games_Get(i);
            Image* image = Games_GetLoadedImage(i);
            game->cover = LoadTextureFromImage(*image);
            UnloadImage(*image);
            isTextureUploaded[i] = true;
        }
        //If image not loaded, it is pending
        else {
            pending = true;
        }
    }
    //If not pending, all images are loaded
    if (!pending) {
        allLoaded = true;
    }
}

//Initialize the states
void States_Init(void) {
    currentConsoleState = STATE_BOOT;
    newConsoleState = STATE_MAIN_MENU;
    bootTimer = 0.0f;
    launchTimer = 0;
    gamesLoaded = 0;
    //Create thread for loading images
    pthread_create(&loadThread, NULL, Games_LoadImages, NULL);
    Var_SetScrollGames(SCROLL_NO);
    Var_SetScrollCateg(SCROLL_NO);
    allLoaded = false;
    is_game_running = false;
    char *mappings = LoadFileText("assets/txt/gamecontrollerdb.txt");
    //If file loaded successfully, set mappings and unload file
    if (mappings != NULL) {
        SetGamepadMappings(mappings);
        UnloadFileText(mappings);
    }
    InitAudioDevice();
    Var_FindThermalZone();
    char color1[COLOR_LEN] = "", color2[COLOR_LEN] = "", color3[COLOR_LEN] = "", bright[BRIGHT_LEN] = "", diag[DIAG_LEN] = "";
    FILE* f = fopen("assets/txt/ui.txt", "r");
    //Check if the file opened successfully
    if (f) {
        //Get theme color 1 and set the variables for the color and the background
        if (fgets(color1, sizeof(color1), f)) {
            color1[strcspn(color1, "\n")] = '\0';
            Var_SetColor1(States_NametoColor(color1));
            Var_SetBackground(Var_NametoBackground(color1));
        }
        //If no text, set default case (blue)
        else {
            Var_SetColor1(BLUE);
            Var_SetBackground(Var_NametoBackground("BLUE"));
        }
        //Get theme color 2 and set the variable
        if (fgets(color2, sizeof(color2), f)) {
            color2[strcspn(color2, "\n")] = '\0';
            Var_SetColor2(States_NametoColor(color2));;
        }
        //If no text, set default case (black)
        else {
            Var_SetColor2(BLACK);
        }
        //Get theme color 3 and set the variable
        if (fgets(color3, sizeof(color3), f)) {
            color3[strcspn(color3, "\n")] = '\0';
            Var_SetColor3(States_NametoColor(color3));;
        }
        //If no text, set default case (white)
        else {
            Var_SetColor3(WHITE);
        }
        //Get brightness and set the variable
        if (fgets(bright, sizeof(bright), f)) {
            bright[strcspn(bright, "\n")] = '\0';
            Var_SetBright(atof(bright));
        }
        //If no text, set default brightness and brightness circle position
        else {
            Var_SetBright(MAX_BRIGHTNESS);
        }
        //Get whether diagnostics are being displayed and set the variable
        if (fgets(diag, sizeof(diag), f)) {
            diag[strcspn(diag, "\n")] = '\0';
            Var_SetDiag(atoi(diag));
        }
        //If no text, set default (don't display)
        else {
            Var_SetDiag(false);
        }
        //Close the file
        fclose(f);
    }
    //Fallback if file doesn't open
    else {
        Var_SetColor1(BLUE);
        Var_SetBackground(Var_NametoBackground("BLUE"));
        Var_SetColor2(BLACK);
        Var_SetColor3(WHITE);
        Var_SetBright(MAX_BRIGHTNESS);
        Var_SetDiag(false);
    }
    //Set max length to start at 0
    maxLen = 0;
    //Go through all games
    for (int i = 0; i < GAMES_LEN; i++) {
        //If game title is larger than max length, set new max length
        if (MeasureTextEx(Var_GetFontBold(), Games_Get(i)->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x > maxLen) {
            maxLen = MeasureTextEx(Var_GetFontBold(), Games_Get(i)->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x;
        }
        //If console title is larger than max length, set new max length
        if (MeasureTextEx(Var_GetFontBold(), Games_Get(i)->console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x > maxLen) {
            maxLen = MeasureTextEx(Var_GetFontBold(), Games_Get(i)->console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x;
        }
    }
    //If length of bottom text is larger than max length, set new max length
    if (MeasureTextEx(Var_GetFontBold(), BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x > maxLen) {
        maxLen = MeasureTextEx(Var_GetFontBold(), BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x;
    }
    UI_LoadLogos();
    UI_SetMaxLen(maxLen);
    UI_LoadControlsImgs();
}

//Update states and variables and draw the correct screen
void States_UpdateAndDraw(void) {
    //Transition
    switch(currentConsoleState) {
        //Console boots up
        case STATE_BOOT:
            //If the boot up time has passed, go to the main menu
            if (bootTimer >= BOOT_TIME) {
                currentConsoleState = STATE_MAIN_MENU;
                UI_Init();
            }
            break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
            //Check if console is transitioning to a new state, and transition accordingly
            if (newConsoleState == STATE_APP_LAUNCHER) {
                currentConsoleState = STATE_LAUNCHING;
                launchTimer = 0;
            }
            else if (newConsoleState == STATE_VIEW_DIAG) {
                Diagnostics_Init();
                currentConsoleState = STATE_VIEW_DIAG;
            }
            break;

        //Launching app
        case STATE_LAUNCHING:
            //If has been launching long enough, transition to officially launch the app
            if (launchTimer >= LAUNCH_TIME) {
                currentConsoleState = STATE_APP_LAUNCHER;
                //Initialize the game and free up the FPS
                Play_Init(Games_GetDisplayed(CURRENT_GAME));
                SetTargetFPS(0);
            }
            break;

        //Running app
        case STATE_APP_LAUNCHER:
            //If game is no longer running, transition back to main menu
            if (!is_game_running) {
                currentConsoleState = STATE_MAIN_MENU;
                newConsoleState = STATE_MAIN_MENU;
                SetTargetFPS(FPS);
                UI_Init();
            }
            break;

        //View diagnostics menu
        case STATE_VIEW_DIAG:
            //Check if console is transitioning to a new state, and transition accordingly
            if (newConsoleState == STATE_MAIN_MENU) {
                currentConsoleState = STATE_MAIN_MENU;
                UI_Init();
            }
            break;
    }

    //Action
    switch(currentConsoleState) {

        //Console boots up
        case STATE_BOOT:
            UI_DrawBootScreen();
            bootTimer += GetFrameTime();
            DrawRectangle(0, 0, Var_GetMonitorWidth(), Var_GetMonitorHeight(), (Color){ 0, 0, 0, Var_GetBright() });
            break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
            UI_Tick(&newConsoleState);
            DrawRectangle(0, 0, Var_GetMonitorWidth(), Var_GetMonitorHeight(), (Color){ 0, 0, 0, Var_GetBright() });
            break;

        //Launching app
        case STATE_LAUNCHING:
            UI_DrawLaunch(Games_GetDisplayed(CURRENT_GAME));
            launchTimer += GetFrameTime();
            DrawRectangle(0, 0, Var_GetMonitorWidth(), Var_GetMonitorHeight(), (Color){ 0, 0, 0, Var_GetBright() });
            break;
            
        //Running app
        case STATE_APP_LAUNCHER:
            //If the game is libretro
            if (Games_GetDisplayed(CURRENT_GAME)->libRetro == true) {
                is_game_running = Play_TickLib(Games_GetDisplayed(CURRENT_GAME));
                DrawRectangle(0, 0, Var_GetMonitorWidth(), Var_GetMonitorHeight(), (Color){ 0, 0, 0, Var_GetBright() });
            }
            else {
                is_game_running = Play_TickExt(Games_GetDisplayed(CURRENT_GAME));
            }
            break;

        //View diagnostics menu
        case STATE_VIEW_DIAG:
            Diagnostics_Tick(&newConsoleState);
            DrawRectangle(0, 0, Var_GetMonitorWidth(), Var_GetMonitorHeight(), (Color){ 0, 0, 0, Var_GetBright() });
            break;
   }
}