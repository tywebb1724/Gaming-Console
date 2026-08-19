#include "theme.h"
#include "controller.h"
#include "var.h"
#include "uipause.h"
#include "config.h"

//Current State of the theme options
static ThemeState currentThemeState;
//Time elapsed on current theme
static float themeTimeElapsed;


//Update the time elapsed on current theme
static void Theme_UpdateTime(void) {
    themeTimeElapsed += GetFrameTime();
    //If time getting too big, bring back to the threshold
    if (themeTimeElapsed > MAX_TIME_ELAPSED) {
        themeTimeElapsed = THRESHOLD_TIME_ELAPSED;
    }
}

//Draw the theme options
static void Theme_Draw(void) {
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
    //Draw select text
    Vector2 selectSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_SELECT, UIPAUSE_SELECT_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 select = {UIPAUSE_SELECT_X, UIPAUSE_SELECT_Y};
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_SELECT, select, UIPAUSE_SELECT_SIZE, UIPAUSE_TITLE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    //Draw back text
    Vector2 backSize = MeasureTextEx(Var_GetFontRegular(), UIPAUSE_BACK, UIPAUSE_BACK_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 back = {UIPAUSE_BACK_X, UIPAUSE_BACK_Y};
    DrawTextEx(Var_GetFontRegular(), UIPAUSE_BACK, back, UIPAUSE_BACK_SIZE, UIPAUSE_TITLE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
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

//Inputs to go down a section
static bool Theme_Down(void) {
    return (IsKeyPressed(KEY_DOWN)) || ((IsKeyDown(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= SELECT_TIME);
}

//Inputs to go up a section
static bool Theme_Up(void) {
    return (IsKeyPressed(KEY_UP)) || ((IsKeyDown(KEY_UP) || LS_UP) && themeTimeElapsed >= SELECT_TIME);
}

//Inputs to select a section
static bool Theme_Select(void) {
    return ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A());
}

//Theme options init function
void Theme_Init(void) {
    themeTimeElapsed = 0;
    currentThemeState = THEME_1;
}

//Theme options tick function
void Theme_Tick(void) {
    //Transition
    switch (currentThemeState) {
        //Blue theme
        case THEME_1:
            //Go up or down themes depending on user input
            if (Theme_Down()) {
                currentThemeState = THEME_2;
                themeTimeElapsed = 0.0f;
            }
            else if (Theme_Up()) {
                currentThemeState = THEME_4;
                themeTimeElapsed = 0.0f;
            }
            break;

        //Red theme
        case THEME_2:
            //Go up or down themes depending on user input
            if (Theme_Down()) {
                currentThemeState = THEME_3;
                themeTimeElapsed = 0.0f;
            }
            else if (Theme_Up()) {
                currentThemeState = THEME_1;
                themeTimeElapsed = 0.0f;
            }
            break;

        //Green theme
        case THEME_3:
            //Go up or down themes depending on user input
            if (Theme_Down()) {
                currentThemeState = THEME_4;
                themeTimeElapsed = 0.0f;
            }
            else if (Theme_Up()) {
                currentThemeState = THEME_2;
                themeTimeElapsed = 0.0f;
            }
            break;

        //Yellow theme
        case THEME_4:
            //Go up or down themes depending on user input
            if (Theme_Down()) {
                currentThemeState = THEME_1;
                themeTimeElapsed = 0.0f;
            }
            else if (Theme_Up()) {
                currentThemeState = THEME_3;
                themeTimeElapsed = 0.0f;
            }
            break;
    }

    //Action
    switch (currentThemeState) {
        //Blue theme
        case THEME_1:
            Theme_Draw();
            Theme_UpdateTime();
            //Set the theme if selected by user
            if (Theme_Select()) {
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
            Theme_Draw();
            Theme_UpdateTime();
            //Set the theme if selected by user
            if (Theme_Select()) {
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
            Theme_Draw();
            Theme_UpdateTime();
            //Set the theme if selected by user
            if (Theme_Select()) {
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
            Theme_Draw();
            Theme_UpdateTime();
            //Set the theme if selected by user
            if (Theme_Select()) {
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