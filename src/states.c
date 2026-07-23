#include "states.h"
#include "raylib.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "categories.h"
#include "play.h"
#include <pthread.h>
#include "var.h"

//State of the console
static ConsoleState currentConsoleState;
static ConsoleState changeConsoleState;
//Timer to keep track of the time on the boot screen
static float bootTimer;
//Variable to keep track of the amount of games loaded
static int gamesLoaded;
//Variable to show whether all the game covers have been loaded
static bool allLoaded;
//Thread for loaded images
static pthread_t loadThread;

static Texture2D controls;


static bool isTextureUploaded[GAMES_LEN] = { false };

static int monitorWidth;
static int monitorHeight;

static bool is_game_running;

//Load all game images (if not loaded already)
void State_LoadGameImages() {
    //If all games aren't dont loading
    if (!allLoaded) {
        //Searc through all the games
        for (int i = 0; i < GAMES_LEN; i++) {
            //If iamge is loaded but texture isn't uplaoded yet
            if (Games_GetIsLoaded(i) && !isTextureUploaded[i]) {
                //Load texture and unload image
                game_t* game = Games_Get(i);
                Image* image = Games_GetLoadedImage(i);
                (*game).cover = LoadTextureFromImage(*image);
                UnloadImage((*image));
                isTextureUploaded[i] = true;
                //If on the last game, all games are loaded
                if (i == GAMES_LEN - 1) {
                    allLoaded = true;
                }
            }
        }
    }
}

//Initialize the states
void State_Init() {
    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);
    //Console starts in boot state
    currentConsoleState = STATE_BOOT;
    //Boot timer starts at 0
    bootTimer = 0.0f;
    //Initially no games loaded
    gamesLoaded = 0;
    //Create thread for loading images
    pthread_create(&loadThread, NULL, Games_LoadImages, NULL);
    //Start in a static position
    scrollGames = SCROLL_NO;
    scrollCategories = SCROLL_NO;
    //All textures not loaded
    allLoaded = false;
    exitApp = false;
    brightness = 0;
    brightCircleX = BRIGHTNESS_LINE_X_END;

    is_game_running = false;

    
    char *mappings = LoadFileText("/home/tywebb1724/Desktop/Gaming-Console/assets/system/gamecontrollerdb.txt");  // raylib's file loader
if (mappings != NULL) {
    SetGamepadMappings(mappings);
    UnloadFileText(mappings);
}
    InitAudioDevice();
    controls = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/controller.png");
}

//Update states and variabels and draw the correct screen
void State_UpdateAndDraw() {

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
                Games_LaunchInit(*Games_GetDisplayed(3));
                SetTargetFPS(0);
            }
            else if (changeConsoleState == STATE_LIST) {
                currentConsoleState = STATE_LIST;
            }
            else if (changeConsoleState == STATE_VIEW_DIAG) {
                currentConsoleState = STATE_VIEW_DIAG;
            }
            break;

        //Launching/running app
        case STATE_APP_LAUNCHER:
            if (Games_GetDisplayed(3)->libRetro == true) {
                if (!is_game_running) {
                    currentConsoleState = STATE_MAIN_MENU;
                    is_game_running = false;
                    SetTargetFPS(60);
                }
            }    
            else {
                currentConsoleState = STATE_MAIN_MENU;
                SetTargetFPS(60);
            }
            break;

        //View consoles/games list
        case STATE_LIST:
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                //mouseWasPressed = true;
                currentConsoleState = STATE_MAIN_MENU;
            }
            break;

        //View diagnostics menu
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
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, Var_GetBrightness() });
            break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
            if (IsKeyPressed(KEY_M)) {
                Games_ClearData(*Games_GetDisplayed(3));
            }
            //Draw UI for the menu
            changeConsoleState = MainMenu_Tick(currentConsoleState);
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, Var_GetBrightness() });
            break;

        //Launching/running app
        case STATE_APP_LAUNCHER:
            if (Games_GetDisplayed(3)->libRetro == true) {
                is_game_running = Play_Tick(*Games_GetDisplayed(3));
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, Var_GetBrightness() });
            }    
            break;

        //View consoles/games list
        case STATE_LIST:
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, Var_GetBrightness() });
            break;

        //View diagnostics menu
        case STATE_VIEW_DIAG:
            changeConsoleState = UI_DrawDiagnostics(currentConsoleState);
            UI_DrawOptionsBtn();
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, Var_GetBrightness() });
            break;
   }
}