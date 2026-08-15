#ifndef UIPAUSE_H
#define UIPAUSE_H

#include "var.h"

#define MAX_TIME_ELAPSED_UIPAUSE 1000000
#define THRESHOLD_TIME_ELAPSED_UIPAUSE 0.25f

//States of the UI pause menu
typedef enum {
    BRIGHTNESS,
    THEME,
    DISPLAY_DIAGNOSTICS,
    VIEW_DIAGNOSTICS,
    POWER_OFF
} UIPauseState;

//Init function for pause menu
void UIPause_Init();
//Tick function for pause menu
void UIPause_Tick(ConsoleState* currentConsoleState);

#endif