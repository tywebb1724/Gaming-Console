#include "states.h"
#include "raylib.h"
#include "ui.h"
#include "games.h"

//Time the boot up screen should display
#define BOOT_TIME 2.0f

static ConsoleState currentConsoleState = STATE_BOOT;

static int menuSelection = 0;
static float bootTimer = 0.0f;

//Initialize the states
void State_Init(void) {
    currentConsoleState = STATE_BOOT;
    menuSelection = 0;
    bootTimer = 0.0f;
}

//Update states and variabels and draw the correct screen
void State_UpdateAndDraw(void) {
    switch(currentConsoleState) {
        //Console boots up
        case STATE_BOOT:
        //Draw the boot up screen
        UI_DrawBootScreen();
        bootTimer += GetFrameTime();
        //If the boot up time has passed, go to the main menu
        if (bootTimer >= BOOT_TIME) {
            Games_LoadTextures();
            UI_LoadTextures();
            currentConsoleState = STATE_MAIN_MENU;
        }
        break;

        //Drawing the main menu
        case STATE_MAIN_MENU:
        UI_DrawMainMenu(menuSelection);
        break;

        case STATE_APP_LAUNCHER:
        break;

        case STATE_DIAGNOSTICS:
        break;
    }
}