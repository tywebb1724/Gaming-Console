#include "uipause.h"
#include "var.h"
#include "raylib.h"
#include "controller_config.h"
#include "uipause_config.h"
#include "config.h"
#include "brightness.h"
#include "theme.h"
#include "diagnostics.h"

//Current state of the options
static UIPauseState currentUIPauseState;
//Time elapsed on the current selected option
static float uipauseTimeElapsed;


//Update the time elapsed on current option
static void UIPause_UpdateTime() {
    //Increment the time elapsed
    uipauseTimeElapsed += GetFrameTime();
    //If time getting too big, bring back to the threshold
    if (uipauseTimeElapsed > 1000000) {
        uipauseTimeElapsed = 0.25f;
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
    //Draw view list
    Vector2 listSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_VIEW_LIST, UIPAUSE_VIEW_LIST_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 list = {UIPAUSE_VIEW_LIST_X, UIPAUSE_VIEW_LIST_Y};
    Rectangle rectList = {
        UIPAUSE_SELECT_RECT_X,
        UIPAUSE_VIEW_LIST_RECT_Y,
        UIPAUSE_SELECT_RECT_W,
        UIPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectList, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_VIEW_LIST, list, UIPAUSE_VIEW_LIST_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
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
    //Draw select text
    Vector2 selectSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_SELECT, UIPAUSE_SELECT_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 select = {UIPAUSE_SELECT_X, UIPAUSE_SELECT_Y};
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_SELECT, select, UIPAUSE_SELECT_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw back text
    Vector2 backSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_BACK, UIPAUSE_BACK_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 back = {UIPAUSE_BACK_X, UIPAUSE_BACK_Y};
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_BACK, back, UIPAUSE_BACK_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw outline on selected section
    if (currentUIPauseState == LIST) {
        DrawRectangleRoundedLinesEx(rectList, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentUIPauseState == BRIGHTNESS) {
        DrawRectangleRoundedLinesEx(rectBright, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentUIPauseState == THEME) {
        DrawRectangleRoundedLinesEx(rectTheme, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentUIPauseState == DISPLAY_DIAGNOSTICS) {
        DrawRectangleRoundedLinesEx(rectDDiag, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else {
        DrawRectangleRoundedLinesEx(rectDiag, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
}

//Init function for pause menu
void UIPause_Init() {
    //Start with the list option selected
    currentUIPauseState = LIST;
    //Start with no time on the selected option
    uipauseTimeElapsed = 0.0f;
    //Not display brightness or theme options
    Var_SetDisplayBright(false);
    Var_SetDisplayTheme(false);
}

//Tick function for pause menu
void UIPause_Tick(ConsoleState* currentConsoleState) {
    //Transition
    switch (currentUIPauseState) {
        case LIST:
            //Move selected option up or down depending on input
            if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyDown(KEY_DOWN) || LS_DOWN) && uipauseTimeElapsed >= 0.2f)) {
                currentUIPauseState = BRIGHTNESS;
                uipauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP)) || ((IsKeyDown(KEY_UP) || LS_UP) && uipauseTimeElapsed >= 0.2f)) {
                currentUIPauseState = VIEW_DIAGNOSTICS;
                uipauseTimeElapsed = 0.0f;
            }
            break;

        case BRIGHTNESS:
            //Move selected option up or down depending on input
            if (((IsKeyPressed(KEY_DOWN)) || ((IsKeyDown(KEY_DOWN) || LS_DOWN) && uipauseTimeElapsed >= 0.2f)) && Var_GetDisplayBright() == false) {
                currentUIPauseState = THEME;
                uipauseTimeElapsed = 0.0f;
            }
            else if (((IsKeyPressed(KEY_UP)) || ((IsKeyDown(KEY_UP) || LS_UP) && uipauseTimeElapsed >= 0.2f)) && Var_GetDisplayBright() == false) {
                currentUIPauseState = LIST;
                uipauseTimeElapsed = 0.0f;
            }
            //Display or stop displaying brightness options
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A() && Var_GetDisplayBright() == false) {
                Brightness_Init();
                Var_SetDisplayBright(true);
                Controller_SetWasPressed_A(true);
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                Var_SetDisplayBright(false);
            }
            break;

        case THEME:
            //Move selected option up or down depending on input
            if (((IsKeyPressed(KEY_DOWN)) || ((IsKeyDown(KEY_DOWN) || LS_DOWN) && uipauseTimeElapsed >= 0.2f)) && Var_GetDisplayTheme() == false) {
                currentUIPauseState = DISPLAY_DIAGNOSTICS;
                uipauseTimeElapsed = 0.0f;
            }
            else if (((IsKeyPressed(KEY_UP)) || ((IsKeyDown(KEY_UP) || LS_UP) && uipauseTimeElapsed >= 0.2f)) && Var_GetDisplayTheme() == false) {
                currentUIPauseState = BRIGHTNESS;
                uipauseTimeElapsed = 0.0f;
            }
            //Display or stop displaying theme options
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A() && Var_GetDisplayTheme() == false) {
                Theme_Init();
                Var_SetDisplayTheme(true);
                Controller_SetWasPressed_A(true);
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                Var_SetDisplayTheme(false);
            }
            break;

        case DISPLAY_DIAGNOSTICS:
            //Move selected option up or down depending on input
            if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyDown(KEY_DOWN) || LS_DOWN) && uipauseTimeElapsed >= 0.2f)) {
                currentUIPauseState = VIEW_DIAGNOSTICS;
                uipauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP)) || ((IsKeyDown(KEY_UP) || LS_UP) && uipauseTimeElapsed >= 0.2f))  {
                currentUIPauseState = THEME;
                uipauseTimeElapsed = 0.0f;
            }
            //Start or stop displaying diagnostics
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
                bool diag = Var_GetDiag();
                Var_SetDiag(!diag);
                Var_UpdateUIFile();
                Controller_SetWasPressed_A(true);
            }
            break;

        case VIEW_DIAGNOSTICS:
            //Move selected option up or down depending on input
            if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyDown(KEY_DOWN) || LS_DOWN) && uipauseTimeElapsed >= 0.2f)) {
                currentUIPauseState = LIST;
                uipauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP)) || ((IsKeyDown(KEY_UP) || LS_UP) && uipauseTimeElapsed >= 0.2f)) {
                currentUIPauseState = DISPLAY_DIAGNOSTICS;
                uipauseTimeElapsed = 0.0f;
            }
            break;
        }

    //Action
    switch (currentUIPauseState) {
        case LIST:
            //Draw pause menu
            UIPause_Draw(currentUIPauseState, *currentConsoleState);
            //Update elapsed time
            UIPause_UpdateTime();
            break;

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
            break;

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
            break;

        case DISPLAY_DIAGNOSTICS:
            //Draw options menu
            UIPause_Draw(currentUIPauseState, *currentConsoleState);
            //Update elapsed time
            UIPause_UpdateTime();
            break;

        case VIEW_DIAGNOSTICS:
            //If user selects this option
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
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
            //Draw options menu
            UIPause_Draw(currentUIPauseState, *currentConsoleState);
            //Update elapsed time
            UIPause_UpdateTime();
            break;
    }
}

