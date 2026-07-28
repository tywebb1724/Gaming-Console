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

//Draw options button at top right of screen
void UI_DrawOptions();
//Draw the boot screen
void UI_DrawBootScreen() ;
//Draw launching screen
void UI_DrawLaunch(const game_t* game);
//Display diagnostics on top left
void UI_DrawDispDiag();
//Tick function for the main UI
void UI_Tick(ConsoleState* currentConsoleState);
//Init function for the main UI
void UI_Init();

#endif