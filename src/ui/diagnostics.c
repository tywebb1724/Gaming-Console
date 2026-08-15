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
    //Update the diagnostics
    Var_UpdateTemp();
    Var_UpdateClock();
    Var_UpdateFrame();
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
    Vector2 frame = {FRAME_TXT_X, FRAME_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), TextFormat("Frame:  %.1f     Worst: %.1f", Var_GetFrameAvg(), Var_GetFrameWorst()), frame, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
    Vector2 temp = {TEMP_TXT_X, TEMP_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), TextFormat("CPU Temperature:  %.1f C", Var_GetTemp()), temp, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
    Vector2 clock = {CLOCK_TXT_X, CLOCK_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), TextFormat("Clock Speed:  %.2f GHz", Var_GetClock() / 1000000.0f, Var_GetClock()), clock, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
    Vector2 refresh = {REFRESH_TXT_X, REFRESH_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), TextFormat("Monitor Refresh Rate:  %d Hz", GetMonitorRefreshRate(0)), refresh, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
    Vector2 control = {CONTROL_TXT_X, CONTROL_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), TextFormat("Controllers connected:  %d", Controller_Count()), control, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
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
            if (IsKeyPressed(KEY_ESCAPE) || HOME_PRESS) {
                currentDiagState = DIAG_OPTIONS;
                UIPause_Init();
            }
            break;

        //Displaying options menu
        case DIAG_OPTIONS:
            //If correct button(s) pressed, take away options menu
            if (HOME_PRESS || ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Var_GetDisplayBright() && !Var_GetDisplayTheme())) {
                currentDiagState = DIAG_NORMAL;
                Controller_SetWasPressed_B(true);
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
            UI_DrawDispDiag(false);
            break;

        //Displaying options menu
        case DIAG_OPTIONS:
            UI_ChangeAlpha_Static();
            //Draw diagnostics screen
            Diagnostics_Draw();
            //Options tick
            UIPause_Tick(currentConsoleState);
            //Draw display diagnostics
            UI_DrawDispDiag(false);
            break;
    }
}
