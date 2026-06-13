#include "states.h"
#include "raylib.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "categories.h"
#include <stdio.h>
#include <pthread.h>

//Time the boot up screen should display
#define BOOT_TIME 3.0f
//State of the console
static ConsoleState currentConsoleState;
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
    spiderLogo = LoadTexture("./assets/covers/logo/SpiderLogo.png");
    //Load fonts
    fontRegular = LoadFont("assets/fonts/Exo2-Regular.ttf");
    fontBold = LoadFont("assets/fonts/Exo2-Bold.ttf");
    //All textures not loaded
    allLoaded = false;
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
            //If not scrolling games to left, not scrolling categories, and the right input is pressed
            if (scrollGames != SCROLL_LEFT && scrollCategories == SCROLL_NO && IsKeyDown(KEY_RIGHT)) {
                //Scroll games to the right
                scrollGames = SCROLL_RIGHT;
            }
            //If not scrolling games to right, not scrolling categories, and the right input is pressed
            else if (scrollGames != SCROLL_RIGHT && scrollCategories == SCROLL_NO && IsKeyDown(KEY_LEFT)) {
                //Scroll games to the left
                scrollGames = SCROLL_LEFT;
            }

            //If not scrolling categories to left and right input is pressed
            if (scrollCategories != SCROLL_LEFT && IsKeyDown(KEY_D)) {
                //If not already scrolling cateogries
                if (scrollCategories == SCROLL_NO) {
                    //Scroll cateogries to right and don't scroll games
                    scrollCategories = SCROLL_RIGHT;
                    scrollGames = SCROLL_NO;
                    //Get new games ready and reset the coordinates
                    Game_New_Indexes();
                    Games_NewRefresh();
                    UI_ResetDisplayCoords_Scroll();
                }
            }
            //If not scrolling categories to right and right input is pressed
            else if (scrollCategories != SCROLL_RIGHT && IsKeyDown(KEY_A)) {
                //If not already scrolling cateogries
                if (scrollCategories == SCROLL_NO) {
                    //Scroll cateogries to right and don't scroll games
                    scrollCategories = SCROLL_LEFT;
                    scrollGames = SCROLL_NO;
                    //Get new games ready and reset the coordinates
                    Game_New_Indexes();
                    Games_NewRefresh();
                    UI_ResetDisplayCoords_Scroll();
                }
            }
            break;

        case STATE_APP_LAUNCHER:
            break;

        case STATE_DIAGNOSTICS:
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
            break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
            //Draw UI for the menu
            UI_DrawMainMenu();
            break;

        case STATE_APP_LAUNCHER:
            break;

        case STATE_DIAGNOSTICS:
            break;
            
    }
}