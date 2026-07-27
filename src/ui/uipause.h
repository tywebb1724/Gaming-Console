#ifndef UIPAUSE_H
#define UIPAUSE_H

#include "var.h"

//States of the UI pause menu
typedef enum {
    LIST,
    BRIGHTNESS,
    THEME,
    DISPLAY_DIAGNOSTICS,
    VIEW_DIAGNOSTICS
} UIPauseState;

//Init function for pause menu
void UIPause_Init();
//Tick function for pause menu
void UIPause_Tick(ConsoleState* currentConsoleState);

#endif