#ifndef UI_H
#define UI_H

#include "games.h"
#include "var.h"

//State of the main UI
typedef enum {
    NORMAL,
    SCROLL_GAMES,
    SCROLL_CATEGORIES,
    OPTIONS
} UIState;

//Set the max length variable
void UI_SetMaxLen(float value);
//Draw options button at top right of screen
void UI_DrawOptions();
//Load spider logo
void UI_LoadLogo();
//Draw the boot screen
void UI_DrawBootScreen();
//Load image of controller
void UI_LoadControlImg();
//Draw launching screen
void UI_DrawLaunch(const game_t* game);
//Display diagnostics on top left
void UI_DrawDispDiag(bool in_game);
//Change alphas when static
void UI_ChangeAlpha_Static();
//Tick function for the main UI
void UI_Tick(ConsoleState* currentConsoleState);
//Init function for the main UI
void UI_Init();

#endif