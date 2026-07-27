#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "var.h"

//Macros for drawing diagnostics screen
#define DIAGNOSTICS_TITLE "System Diagnostics"
#define DIAGNOSTICS_TITLE_SIZE (Var_GetMonitorWidth() / 25.0f)
#define DIAGNOSTICS_TITLE_X (CENTER_X - diagnosticsSize.x / 2)
#define DIAGNOSTICS_TITLE_Y (Var_GetMonitorHeight() / 12.0f)
#define DIAGNOSTICS_TITLE_TXT_Y (DIAGNOSTICS_TITLE_Y - diagnosticsSize.y / 2)
#define DIAGNOSTICS_SIZE (Var_GetMonitorWidth() / 35.0f)
#define DIAGNOSTICS_SPACING (Var_GetMonitorWidth() / 400.0f)
#define RESOLUTION_TXT_X (FPS_TXT_X)
#define RESOLUTION_TXT_Y (Var_GetMonitorHeight() / 4.0f)
#define FPS_TXT_X (Var_GetMonitorWidth() / 10.0f)
#define FPS_TXT_Y (RESOLUTION_TXT_Y + Var_GetMonitorHeight() / 12.0f)

//State of the diagnostics screen
typedef enum {
    DIAG_NORMAL,
    DIAG_OPTIONS
} DiagState;

//Diagnostics screen init function
void Diagnostics_Init();
//Diagnostics screen tick function
void Diagnostics_Tick(ConsoleState* currentConsoleState);

#endif