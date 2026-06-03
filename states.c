#include "states.h"
#include "raylib.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "consoles.h"
#include <stdio.h>

//Time the boot up screen should display
#define BOOT_TIME 2.0f

static ConsoleState currentConsoleState = STATE_BOOT;

static float bootTimer = 0.0f;

MenuState currentMenuState = CONSOLES;

//Initialize the states
void State_Init() {
    currentConsoleState = STATE_BOOT;
    bootTimer = 0.0f;
    UI_ResetDisplayCoords_Games();
    UI_ResetDisplayCoords_Consoles();

    printf("%f\n", consolesDisplayed[2].y);
    printf("%f\n", consolesDisplayed[2].h);
    printf("%f\n", consolesDisplayed[2].w);
    printf("%f\n", consolesDisplayed[2].x);
}

//Update states and variabels and draw the correct screen
void State_UpdateAndDraw() {
    switch(currentConsoleState) {
        //Console boots up
        case STATE_BOOT:
        //Draw the boot up screen
        UI_DrawBootScreen();
        bootTimer += GetFrameTime();
        //If the boot up time has passed, go to the main menu
        if (bootTimer >= BOOT_TIME) {
            Games_LoadTextures();
            Consoles_LoadTextures();
            currentConsoleState = STATE_MAIN_MENU;
        }
        break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
        UI_DrawMainMenu();
        if (scroll == SCROLL_NO && IsKeyPressed(KEY_RIGHT)) {
            scroll = SCROLL_RIGHT;
        }
        else if (scroll == SCROLL_NO && IsKeyPressed(KEY_LEFT)) {
            scroll = SCROLL_LEFT;
        }
        break;

        case STATE_APP_LAUNCHER:
        break;

        case STATE_DIAGNOSTICS:
        break;
    }
}