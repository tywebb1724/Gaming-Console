#include "uipause.h"
#include "var.h"
#include "raylib.h"
#include "controller.h"
#include "config.h"
#include "brightness.h"
#include "theme.h"
#include "diagnostics.h"

//Current state of the options
static UIPauseState currentUIPauseState;
//Time elapsed on the current selected option
static float uipauseTimeElapsed;


//Update the time elapsed on current option
static void UIPause_UpdateTime(void) {
    uipauseTimeElapsed += GetFrameTime();
    //If time getting too big, bring back to the threshold
    if (uipauseTimeElapsed > MAX_TIME_ELAPSED_UIPAUSE) {
        uipauseTimeElapsed = THRESHOLD_TIME_ELAPSED_UIPAUSE;
    }
}

//Draw the pause menu
static void UIPause_Draw(UIPauseState currentUIPauseState, ConsoleState currentConsoleState) {
    //Draw whole section
    Rectangle rectSec = {
        UIPAUSE_RECT_X,
        UIPAUSE_RECT_Y,
        UIPAUSE_RECT_W,
        UIPAUSE_RECT_H};
    DrawRectangleRounded(rectSec, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, Var_GetColor2());
    DrawRectangleRoundedLinesEx(rectSec, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    //Draw title
    Vector2 titleSize = MeasureTextEx(Var_GetFontBold(), UIPAUSE_TITLE, UIPAUSE_TITLE_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 title = {UIPAUSE_TITLE_X, UIPAUSE_TITLE_Y};
    DrawTextEx(Var_GetFontBold(), UIPAUSE_TITLE, title, UIPAUSE_TITLE_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw brightness section
    Vector2 brightnessSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_BRIGHTNESS, UIPAUSE_BRIGHTNESS_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 brightness = {UIPAUSE_BRIGHTNESS_X, UIPAUSE_BRIGHTNESS_Y};
    Rectangle rectBright = {
        UIPAUSE_SELECT_RECT_X,
        UIPAUSE_BRIGHTNESS_RECT_Y,
        UIPAUSE_SELECT_RECT_W,
        UIPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectBright, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_BRIGHTNESS, brightness, UIPAUSE_BRIGHTNESS_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw theme section
    Vector2 themeSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_THEME, UIPAUSE_THEME_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 theme = {UIPAUSE_THEME_X, UIPAUSE_THEME_Y};
    Rectangle rectTheme = {
        UIPAUSE_SELECT_RECT_X,
        UIPAUSE_THEME_RECT_Y,
        UIPAUSE_SELECT_RECT_W,
        UIPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectTheme, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_THEME, theme, UIPAUSE_THEME_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw display diagnostics section
    Vector2 displayDiagSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_DISPLAY_DIAGNOSTICS, UIPAUSE_DISPLAY_DIAGNOSTICS_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 displayDiag = {UIPAUSE_DISPLAY_DIAGNOSTICS_X, UIPAUSE_DISPLAY_DIAGNOSTICS_Y};
    Rectangle rectDDiag = {
        UIPAUSE_SELECT_RECT_X,
        UIPAUSE_DISPLAY_DIAGNOSTICS_RECT_Y,
        UIPAUSE_SELECT_RECT_W,
        UIPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDDiag, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    char *diagTxt;
    //Change text depending on if diagnostics currently being displayee
    if (Var_GetDiag()) {
        diagTxt = UIPAUSE_HIDE_DIAGNOSTICS;
    }
    else {
        diagTxt = UIPAUSE_DISPLAY_DIAGNOSTICS;
    }
    DrawTextEx(Var_GetFontRegular(), diagTxt, displayDiag, UIPAUSE_DISPLAY_DIAGNOSTICS_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw view diagnostics section
    Vector2 diagSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_DIAGNOSTICS, UIPAUSE_DIAGNOSTICS_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 diag = {UIPAUSE_DIAGNOSTICS_X, UIPAUSE_DIAGNOSTICS_Y};
    Rectangle rectDiag = {
        UIPAUSE_SELECT_RECT_X,
        UIPAUSE_DIAGNOSTICS_RECT_Y,
        UIPAUSE_SELECT_RECT_W,
        UIPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDiag, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    char *menuTxt;
    //Change text depending on if on main menu or diagnostics screen
    if (currentConsoleState == STATE_VIEW_DIAG) {
        menuTxt = UIPAUSE_MAIN;
    }
    else {
        menuTxt = UIPAUSE_DIAGNOSTICS;
    }
    DrawTextEx(Var_GetFontRegular(), menuTxt, diag, UIPAUSE_DIAGNOSTICS_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw power off section
    Vector2 powerOffSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_POWER_OFF, UIPAUSE_POWER_OFF_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 powerOff = {UIPAUSE_POWER_OFF_X, UIPAUSE_POWER_OFF_Y};
    Rectangle rectPowerOff = {
        UIPAUSE_SELECT_RECT_X,
        UIPAUSE_POWER_OFF_RECT_Y,
        UIPAUSE_SELECT_RECT_W,
        UIPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectPowerOff, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_POWER_OFF, powerOff, UIPAUSE_POWER_OFF_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw select text
    Vector2 selectSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_SELECT, UIPAUSE_SELECT_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 select = {UIPAUSE_SELECT_X, UIPAUSE_SELECT_Y};
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_SELECT, select, UIPAUSE_SELECT_SIZE, UIPAUSE_TITLE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    //Draw back text
    Vector2 backSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_BACK, UIPAUSE_BACK_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 back = {UIPAUSE_BACK_X, UIPAUSE_BACK_Y};
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_BACK, back, UIPAUSE_BACK_SIZE, UIPAUSE_TITLE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    //Draw outline on selected section
    if (currentUIPauseState == BRIGHTNESS) {
        DrawRectangleRoundedLinesEx(rectBright, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentUIPauseState == THEME) {
        DrawRectangleRoundedLinesEx(rectTheme, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentUIPauseState == DISPLAY_DIAGNOSTICS) {
        DrawRectangleRoundedLinesEx(rectDDiag, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentUIPauseState == VIEW_DIAGNOSTICS) {
        DrawRectangleRoundedLinesEx(rectDiag, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else {
        DrawRectangleRoundedLinesEx(rectPowerOff, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
}

//Inputs to go down a section
static bool UIPause_Down(void) {
    return ((IsKeyPressed(KEY_DOWN)) || ((IsKeyDown(KEY_DOWN) || LS_DOWN) && uipauseTimeElapsed >= SELECT_TIME));
}

//Inputs to go up a section
static bool UIPause_Up(void) {
    return ((IsKeyPressed(KEY_UP)) || ((IsKeyDown(KEY_UP) || LS_UP) && uipauseTimeElapsed >= SELECT_TIME));
}

//Inputs to select a section
static bool UIPause_Select(void) {
    return (IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A();
}

//Init function for pause menu
void UIPause_Init(void) {
    currentUIPauseState = BRIGHTNESS;
    uipauseTimeElapsed = 0.0f;
    Var_SetDisplayBright(false);
    Var_SetDisplayTheme(false);
}

//Tick function for pause menu
void UIPause_Tick(ConsoleState* currentConsoleState) {
    //Transition
    switch (currentUIPauseState) {
        //Brightness section selected
        case BRIGHTNESS:
            //Move selected option up or down depending on input
            if (UIPause_Down() && Var_GetDisplayBright() == false) {
                currentUIPauseState = THEME;
                uipauseTimeElapsed = 0.0f;
            }
            else if (UIPause_Up() && Var_GetDisplayBright() == false) {
                currentUIPauseState = POWER_OFF;
                uipauseTimeElapsed = 0.0f;
            }
            break;

        //Theme section selected
        case THEME:
            //Move selected option up or down depending on input
            if (UIPause_Down() && Var_GetDisplayTheme() == false) {
                currentUIPauseState = DISPLAY_DIAGNOSTICS;
                uipauseTimeElapsed = 0.0f;
            }
            else if (UIPause_Up() && Var_GetDisplayTheme() == false) {
                currentUIPauseState = BRIGHTNESS;
                uipauseTimeElapsed = 0.0f;
            }
            break;

        //Display diagnostics section selected
        case DISPLAY_DIAGNOSTICS:
            //Move selected option up or down depending on input
            if (UIPause_Down()) {
                currentUIPauseState = VIEW_DIAGNOSTICS;
                uipauseTimeElapsed = 0.0f;
            }
            else if (UIPause_Up())  {
                currentUIPauseState = THEME;
                uipauseTimeElapsed = 0.0f;
            }
            break;

        //View diagnostics menu section selected
        case VIEW_DIAGNOSTICS:
            //Move selected option up or down depending on input
            if (UIPause_Down()) {
                currentUIPauseState = POWER_OFF;
                uipauseTimeElapsed = 0.0f;
            }
            else if (UIPause_Up()) {
                currentUIPauseState = DISPLAY_DIAGNOSTICS;
                uipauseTimeElapsed = 0.0f;
            }
            break;

        //Power off section selected
        case POWER_OFF:
            //Move selected option up or down depending on input
            if (UIPause_Down()) {
                currentUIPauseState = BRIGHTNESS;
                uipauseTimeElapsed = 0.0f;
            }
            else if (UIPause_Up()) {
                currentUIPauseState = VIEW_DIAGNOSTICS;
                uipauseTimeElapsed = 0.0f;
            }
        }

    //Action
    switch (currentUIPauseState) {
        //Brightness section selected
        case BRIGHTNESS:
            //Display brightness options or normal options
            if (Var_GetDisplayBright() == true) {
                Brightness_Tick();
            }
            else {
                UIPause_Draw(currentUIPauseState, *currentConsoleState);
                //Update elapsed time
                UIPause_UpdateTime();
            }
            //Display or stop displaying brightness options
            if (UIPause_Select() && Var_GetDisplayBright() == false) {
                Brightness_Init();
                Var_SetDisplayBright(true);
                Controller_SetWasPressed_A(true);
            }
            else if (IsKeyPressed(KEY_ESCAPE) || B_PRESS) {
                Controller_SetWasPressed_B(true);
                Var_SetDisplayBright(false);
            }
            break;

        //Theme section selected
        case THEME:
            //Display theme options or normal options
            if (Var_GetDisplayTheme() == true) {
                Theme_Tick();
            }
            else {
                UIPause_Draw(currentUIPauseState, *currentConsoleState);
                //Update elapsed time
                UIPause_UpdateTime();
            }
            //Display or stop displaying theme options
            if (UIPause_Select() && Var_GetDisplayTheme() == false) {
                Theme_Init();
                Var_SetDisplayTheme(true);
                Controller_SetWasPressed_A(true);
            }
            else if (IsKeyPressed(KEY_ESCAPE) || B_PRESS) {
                Var_SetDisplayTheme(false);
                Controller_SetWasPressed_B(true);
            }
            break;

        //Display diagnostics section selected
        case DISPLAY_DIAGNOSTICS:
            //Start or stop displaying diagnostics
            if (UIPause_Select()) {
                bool diag = Var_GetDiag();
                Var_SetDiag(!diag);
                Var_UpdateUIFile();
                Controller_SetWasPressed_A(true);
            }
            UIPause_Draw(currentUIPauseState, *currentConsoleState);
            UIPause_UpdateTime();
            break;

        //View diagnostics menu section selected
        case VIEW_DIAGNOSTICS:
            //If user selects this option
            if (UIPause_Select()) {
                //Change to the main menu or view diagnostics menu depending on which screen is being displayed
                if (*currentConsoleState == STATE_VIEW_DIAG) {
                    *currentConsoleState = STATE_MAIN_MENU;
                }
                else {
                    Diagnostics_Init();
                    *currentConsoleState = STATE_VIEW_DIAG;
                }
                Controller_SetWasPressed_A(true);
            }
            UIPause_Draw(currentUIPauseState, *currentConsoleState);
            UIPause_UpdateTime();
            break;
        
        //Power off section selected
        case POWER_OFF:
            //Power off
            if (UIPause_Select()) {
                Controller_SetWasPressed_A(true);
                Var_SetPowerOff(true); 
            }
            UIPause_Draw(currentUIPauseState, *currentConsoleState);
            UIPause_UpdateTime();
            
    }
}

