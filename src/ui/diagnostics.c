#include "diagnostics.h"
#include "var.h"
#include "config.h"
#include "controller_config.h"
#include "uipause.h"
#include "ui.h"

//Current state of the diagnostics screen
static DiagState currentDiagState;

//Draw diagnostics screen
static void Diagnostics_Draw() {
    //Clear background
    ClearBackground(BACKGROUND_CLR);
    //Draw title
    Vector2 diagnosticsSize = MeasureTextEx(Var_GetFontBold(), DIAGNOSTICS_TITLE, DIAGNOSTICS_TITLE_SIZE, DIAGNOSTICS_SPACING);
    Vector2 diagnostics = {DIAGNOSTICS_TITLE_X, DIAGNOSTICS_TITLE_Y};
    DrawTextEx(Var_GetFontBold(), DIAGNOSTICS_TITLE, diagnostics, DIAGNOSTICS_TITLE_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
    //Draw diagnostics statistics
    Vector2 resolution = {RESOLUTION_TXT_X, RESOLUTION_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), TextFormat("Resolution:  %d:%d", GetMonitorWidth(0), GetMonitorHeight(0)), resolution, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
    Vector2 fps = {FPS_TXT_X, FPS_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), TextFormat("FPS:  %d", GetFPS()), fps, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
    //Draw options button
    UI_DrawOptions();
}

//Diagnostics screen init function
void Diagnostics_Init() {
    //Start showing only the diagnostics screen
    currentDiagState = DIAG_NORMAL;
}

//Diagnostics screen tick function
void Diagnostics_Tick(ConsoleState* currentConsoleState) {
    //Transition
    switch (currentDiagState) {
        //Displaying diagnostics screen
        case DIAG_NORMAL:
            //If pause button pressed, display options
            if (IsKeyPressed(KEY_TAB) || START_PRESS) {
                currentDiagState = DIAG_OPTIONS;
                UIPause_Init();
            }
            break;

        //Displaying options menu
        case DIAG_OPTIONS:
            //If correct button(s) pressed, take away options menu
            if (IsKeyPressed(KEY_TAB) || START_PRESS || ((IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) && !Var_GetDisplayBright() && !Var_GetDisplayTheme())) {
                currentDiagState = DIAG_NORMAL;
            }
            break;
    }

    //Action
    switch (currentDiagState) {
        //Displaying diagnostics screen
        case DIAG_NORMAL:
            //Draw diagnostics screen
            Diagnostics_Draw();
            //Draw display diagnostics
            UI_DrawDispDiag();
            break;

        //Displaying options menu
        case DIAG_OPTIONS:
            UI_ChangeAlpha_Static();
            //Draw diagnostics screen
            Diagnostics_Draw();
            //Options tick
            UIPause_Tick(currentConsoleState);
            //Draw display diagnostics
            UI_DrawDispDiag();
            break;
    }
}
