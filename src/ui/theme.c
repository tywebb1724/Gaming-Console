#include "theme.h"
#include "controller_config.h"
#include "var.h"
#include "uipause_config.h"
#include "config.h"

//Current State of the theme options
static ThemeState currentThemeState;
//Time elapsed on current theme
static float themeTimeElapsed;


//Update the time elapsed on current theme
static void Theme_UpdateTime() {
    //Increment the time elapsed
    themeTimeElapsed += GetFrameTime();
    //If time getting too big, bring back to the threshold
    if (themeTimeElapsed > 1000000) {
        themeTimeElapsed = 0.25f;
    }
}

//Draw the theme options
static void Theme_Draw() {
    //Draw whole section
    Rectangle rectSec = {
        UIPAUSE_RECT_X,
        UIPAUSE_RECT_Y,
        UIPAUSE_RECT_W,
        UIPAUSE_RECT_H};
    DrawRectangleRounded(rectSec, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, Var_GetColor2());
    DrawRectangleRoundedLinesEx(rectSec, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    //Draw title
    Vector2 themeSize = MeasureTextEx(Var_GetFontBold(), THEME_TXT, THEME_TXT_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 theme = {THEME_TXT_X, THEME_TXT_Y};
    DrawTextEx(Var_GetFontBold(), THEME_TXT, theme, THEME_TXT_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Theme 1
    Rectangle rect1 = {
        THEME_RECT_X,
        THEME_RECT_1_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLUE);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect1, THICKNESS_UIPAUSE, Var_GetColor3());
    //Theme 2
    Rectangle rect2 = {
        THEME_RECT_X,
        THEME_RECT_2_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, RED);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect2, THICKNESS_UIPAUSE, Var_GetColor3());
    //Theme 3
    Rectangle rect3 = {
        THEME_RECT_X,
        THEME_RECT_3_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, GREEN);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect3, THICKNESS_UIPAUSE, Var_GetColor3());
    //Theme 4
    Rectangle rect4 = {
        THEME_RECT_X,
        THEME_RECT_4_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, YELLOW);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect4, THICKNESS_UIPAUSE, Var_GetColor3());
    //Depending on the current theme selected, draw the selected outline
    if (currentThemeState == THEME_1) {
        DrawRectangleLinesEx(rect1, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentThemeState == THEME_2) {
        DrawRectangleLinesEx(rect2, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentThemeState == THEME_3) {
        DrawRectangleLinesEx(rect3, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
    else if (currentThemeState == THEME_4) {
        DrawRectangleLinesEx(rect4, 2 * THICKNESS_UIPAUSE, Var_GetColor1());
    }
}

//Theme options init function
void Theme_Init() {
    //Set time on the current theme to 0
    themeTimeElapsed = 0;
    //Start the theme options on the first theme
    currentThemeState = THEME_1;
}

//Theme options tick function
void Theme_Tick() {
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_DOWN) {
        //mouseWasPressed = false;
    }

    //Transition
    switch (currentThemeState) {
    //Blue theme
    case THEME_1:
        //Go up or down themes depending on user input
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= 0.2f)) {
            currentThemeState = THEME_2;
            themeTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && themeTimeElapsed >= 0.2f)) {
            currentThemeState = THEME_4;
            themeTimeElapsed = 0.0f;
        }
        break;

    //Red theme
    case THEME_2:
        //Go up or down themes depending on user input
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= 0.2f)) {
            currentThemeState = THEME_3;
            themeTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && themeTimeElapsed >= 0.2f)) {
            currentThemeState = THEME_1;
            themeTimeElapsed = 0.0f;
        }
        break;

    //Green theme
    case THEME_3:
        //Go up or down themes depending on user input
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= 0.2f)) {
            currentThemeState = THEME_4;
            themeTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && themeTimeElapsed >= 0.2f)) {
            currentThemeState = THEME_2;
            themeTimeElapsed = 0.0f;
        }
        break;

    //Yellow theme
    case THEME_4:
        //Go up or down themes depending on user input
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= 0.2f)) {
            currentThemeState = THEME_1;
            themeTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && themeTimeElapsed >= 0.2f)) {
            currentThemeState = THEME_3;
            themeTimeElapsed = 0.0f;
        }
        break;
    }

    //Action
    switch (currentThemeState) {
    //Blue theme
    case THEME_1:
        //Draw the menu
        Theme_Draw();
        //Increase the time the current theme has been selected
        Theme_UpdateTime();
        //Set the theme if selected by user
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
            Var_SetColor1(BLUE);
            Var_SetColor2(BLACK);
            Var_SetColor3(WHITE);
            Var_SetBackground(Var_NametoBackground("BLUE"));
            Var_UpdateUIFile();
            Controller_SetWasPressed_A(true);
        }
        break;

    //Red theme
    case THEME_2:
        //Draw the menu
        Theme_Draw();
        //Increase the time the current theme has been selected
        Theme_UpdateTime();
        //Set the theme if selected by user
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
            Var_SetColor1(RED);
            Var_SetColor2(BLACK);
            Var_SetColor3(WHITE);
            Var_SetBackground(Var_NametoBackground("RED"));
            Var_UpdateUIFile();
            Controller_SetWasPressed_A(true);
        }
        break;

    //Green theme
    case THEME_3:
        //Draw the menu
        Theme_Draw();
        //Increase the time the current theme has been selected
        Theme_UpdateTime();
        //Set the theme if selected by user
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
            Var_SetColor1(GREEN);
            Var_SetColor2(BLACK);
            Var_SetColor3(WHITE);
            Var_SetBackground(Var_NametoBackground("GREEN"));
            Var_UpdateUIFile();
            Controller_SetWasPressed_A(true);
        }
        break;

    //Yellow theme
    case THEME_4:
        //Draw the menu
        Theme_Draw();
        //Increase the time the current theme has been selected
        Theme_UpdateTime();
        //Set the theme if selected by user
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
            Var_SetColor1(YELLOW);
            Var_SetColor2(BLACK);
            Var_SetColor3(WHITE);
            Var_SetBackground(Var_NametoBackground("YELLOW"));
            Var_UpdateUIFile();
            Controller_SetWasPressed_A(true);
        }
        break;
    }
}