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

static ConsoleState currentConsoleState = STATE_BOOT;

static float bootTimer = 0.0f;

int gamesLoaded;

pthread_t loadThread;
Texture2D spiderLogo;

bool allLoaded = false;

//Initialize the states
void State_Init() {
    currentConsoleState = STATE_BOOT;
    bootTimer = 0.0f;
    gamesLoaded = 0;
    UI_ResetDisplayCoords_Games();
    pthread_create(&loadThread, NULL, Games_LoadImages, NULL);
    spiderLogo = LoadTexture("./assets/SpiderLogo.png");
}

//Update states and variabels and draw the correct screen
void State_UpdateAndDraw() {
    
    if (!allLoaded) {
        for (int i = 0; i < GAMES_LEN; i++) {
            if (isLoaded[i] && !isTextureUploaded[i]) {
                gameLibrary[i].cover = LoadTextureFromImage(LoadedImages[i]);
                UnloadImage(LoadedImages[i]);
                isTextureUploaded[i] = true;
                if (i == GAMES_LEN - 1) {
                    allLoaded = true;
                    printf("done\n");
                }
                printf("%d\n", i);
                printf("time: %f\n", bootTimer);
            }
        }
    }

    switch(currentConsoleState) {
        //Console boots up
        case STATE_BOOT:
        //Draw the boot up screen
        UI_DrawBootScreen();
        bootTimer += GetFrameTime();
        //If the boot up time has passed, go to the main menu
        if (bootTimer >= BOOT_TIME) {
            currentConsoleState = STATE_MAIN_MENU;
        }
        break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
        UI_DrawMainMenu();
        if (scrollGames == SCROLL_NO && scrollCategories == SCROLL_NO && IsKeyPressed(KEY_RIGHT)) {
            scrollGames = SCROLL_RIGHT;
        }
        else if (scrollGames == SCROLL_NO && scrollCategories == SCROLL_NO && IsKeyPressed(KEY_LEFT)) {
            scrollGames = SCROLL_LEFT;
        }

        if (scrollCategories == SCROLL_NO && IsKeyPressed(KEY_D)) {
            scrollCategories = SCROLL_RIGHT;
            scrollGames = SCROLL_NO;
        }
        else if (scrollCategories == SCROLL_NO && IsKeyPressed(KEY_A)) {
            scrollCategories = SCROLL_LEFT;
            scrollGames = SCROLL_NO;
        }
        break;

        case STATE_APP_LAUNCHER:
        break;

        case STATE_DIAGNOSTICS:
        break;
    }
}