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

//States of the console
typedef enum {
    STATE_BOOT,
    STATE_MAIN_MENU,
    STATE_APP_LAUNCHER,
    STATE_LIST,
    STATE_VIEW_DIAG
} ConsoleState;

//Init function for pause menu
void UIPause_Init(bool *displayBrightness, bool *displayTheme);
//Tick function for pause menu
void UIPause_Tick(ConsoleState* currentConsoleState, bool *displayBrightness, bool *displayTheme);

#endif