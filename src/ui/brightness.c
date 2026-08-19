#include "brightness.h"
#include "raylib.h"
#include "controller.h"
#include "uipause.h"
#include "config.h"
#include <stdio.h>

//Current state of the brightness options screen
static BrightnessState currentBrightnessState;

//Draw the brightness options screen
static void Brightness_Draw(void) {
    //Draw whole section
    Rectangle rect1 = {
        UIPAUSE_RECT_X,
        UIPAUSE_RECT_Y,
        UIPAUSE_RECT_W,
        UIPAUSE_RECT_H};
    DrawRectangleRounded(rect1, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, Var_GetColor2());
    DrawRectangleRoundedLinesEx(rect1, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    //Draw title
    Vector2 brightnessSize = MeasureTextEx(Var_GetFontBold(), BRIGHTNESS_TXT, BRIGHTNESS_TXT_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 brightness = {BRIGHTNESS_TXT_X, BRIGHTNESS_TXT_Y};
    DrawTextEx(Var_GetFontBold(), BRIGHTNESS_TXT, brightness, BRIGHTNESS_TXT_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw line
    DrawLineEx((Vector2){BRIGHTNESS_LINE_X_START, BRIGHTNESS_LINE_Y}, (Vector2){BRIGHTNESS_LINE_X_END, BRIGHTNESS_LINE_Y}, BRIGHTNESS_LINE_THICK, Var_GetColor1());
    //Draw position of brightness circle
    DrawCircle(BRIGHTNESS_CIRCLE_X, BRIGHTNESS_LINE_Y, BRIGHTNESS_RADIUS, Var_GetColor1());
    //Draw arrows
    DrawCircle(BRIGHT_LS_LEFT_X + (Var_GetMonitorWidth() / 700.0f), BRIGHT_LS_Y, BRIGHT_LS_RADIUS, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    Vector2 point1 = {BRIGHT_LS_LEFT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y};
    Vector2 point2 = {BRIGHT_LS_LEFT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y + BRIGHT_LS_TRIANGLE_SIZE / 2};
    Vector2 point3 = {BRIGHT_LS_LEFT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y - BRIGHT_LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point1, point2, point3, Fade(Var_GetColor2(), Var_GetAlphaSelect()));
    DrawCircle(BRIGHT_LS_RIGHT_X - (Var_GetMonitorWidth() / 700.0f), BRIGHT_LS_Y, BRIGHT_LS_RADIUS, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    Vector2 point4 = {BRIGHT_LS_RIGHT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y};
    Vector2 point5 = {BRIGHT_LS_RIGHT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y + BRIGHT_LS_TRIANGLE_SIZE / 2};
    Vector2 point6 = {BRIGHT_LS_RIGHT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y - BRIGHT_LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point6, point5, point4, Fade(Var_GetColor2(), Var_GetAlphaSelect()));
    //Draw percentage
    char percentTxt[PERCENT_STR_LEN];
    int brightPercent = (int)((MIN_BRIGHTNESS - Var_GetBright()) / (MIN_BRIGHTNESS / 100.0f));
    snprintf(percentTxt, sizeof(percentTxt), "%d%%", brightPercent);
    Vector2 brightnessPercentSize = MeasureTextEx(Var_GetFontBold(), percentTxt, BRIGHTNESS_PERCENT_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 brightnessPercent = {BRIGHTNESS_PERCENT_X, BRIGHTNESS_PERCENT_Y};
    DrawTextEx(Var_GetFontBold(), percentTxt, brightnessPercent, BRIGHTNESS_PERCENT_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
}

//Brightness init function
void Brightness_Init(void) {
    currentBrightnessState = NEITHER;
}

//Brightness tick function
void Brightness_Tick(void) {
    //Transition
    switch (currentBrightnessState) {
        //Not increasing or decreasing brightness
        case NEITHER:
            //If pressing to the left, decrease brightness
            if ((IsKeyDown(KEY_LEFT) || LS_LEFT) && Var_GetBright() < MIN_BRIGHTNESS) {
                currentBrightnessState = LEFT_PRESS;
            }
            //If pressing to the right, increase brightness
            else if ((IsKeyDown(KEY_RIGHT) || LS_RIGHT) && Var_GetBright() > MAX_BRIGHTNESS) {
                currentBrightnessState = RIGHT_PRESS;
            }
            break;
        
        //Decreasing brightness
        case LEFT_PRESS:
            //If no longer pressing to the left, stop changing brightness
            if (!IsKeyDown(KEY_LEFT) && !LS_LEFT) {
                currentBrightnessState = NEITHER;
                Var_UpdateUIFile();
            }
            break;

        //Increasing brightness
        case RIGHT_PRESS:
            //If no longer pressing to the right, stop changing brightness
            if (!IsKeyDown(KEY_RIGHT) && !LS_RIGHT) {
                currentBrightnessState = NEITHER;
                Var_UpdateUIFile();
            }
            break;
    }

    //Action
    switch (currentBrightnessState) {
        //Not increasing or decreasing brightness
        case NEITHER:
            Brightness_Draw();
            break;

        //Decreasing brightness
        case LEFT_PRESS:
            Brightness_Draw();
            Var_AddBright(BRIGHTNESS_INCREMENT);
            break;

        //Increasing brightness
        case RIGHT_PRESS:
            Brightness_Draw();
            Var_AddBright(- BRIGHTNESS_INCREMENT);
            break;
    }
}
