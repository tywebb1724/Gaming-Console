#include "brightness.h"
#include "raylib.h"
#include "controller_config.h"
#include "uipause_config.h"
#include "config.h"

//Current state of the brightness options screen
static BrightnessState currentBrightnessState;

//Draw the brightness options screen
static void Brightness_Draw() {
    //Draw whole section
    Rectangle rect1 = {
        UIPAUSE_RECT_X,
        UIPAUSE_RECT_Y,
        UIPAUSE_RECT_W,
        UIPAUSE_RECT_H};
    DrawRectangleRoundedLinesEx(rect1, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, THICKNESS_UIPAUSE, Var_GetColor3());
    DrawRectangleRounded(rect1, UIPAUSE_ROUND, UIPAUSE_SEGMENTS, Var_GetColor2());
    //Draw title
    Vector2 brightnessSize = MeasureTextEx(Var_GetFontBold(), BRIGHTNESS_TXT, BRIGHTNESS_TXT_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 brightness = {BRIGHTNESS_TXT_X, BRIGHTNESS_TXT_Y};
    DrawTextEx(Var_GetFontBold(), BRIGHTNESS_TXT, brightness, BRIGHTNESS_TXT_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());
    //Draw line
    DrawLineEx((Vector2){BRIGHTNESS_LINE_X_START, BRIGHTNESS_LINE_Y}, (Vector2){BRIGHTNESS_LINE_X_END, BRIGHTNESS_LINE_Y}, BRIGHTNESS_LINE_THICK, Var_GetColor1());
    //Draw position of brightness circle
    DrawCircle(Var_GetBrightX(), BRIGHTNESS_LINE_Y, BRIGHTNESS_RADIUS, Var_GetColor1());
    //Draw arrows
    DrawCircle(BRIGHT_LS_LEFT_X + (SCREEN_W / 700.0f), BRIGHT_LS_Y, BRIGHT_LS_RADIUS, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    Vector2 point1 = {BRIGHT_LS_LEFT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y};
    Vector2 point2 = {BRIGHT_LS_LEFT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y + BRIGHT_LS_TRIANGLE_SIZE / 2};
    Vector2 point3 = {BRIGHT_LS_LEFT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y - BRIGHT_LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point1, point2, point3, Fade(Var_GetColor2(), Var_GetAlphaSelect()));
    DrawCircle(BRIGHT_LS_RIGHT_X - (SCREEN_W / 700.0f), BRIGHT_LS_Y, BRIGHT_LS_RADIUS, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    Vector2 point4 = {BRIGHT_LS_RIGHT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y};
    Vector2 point5 = {BRIGHT_LS_RIGHT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y + BRIGHT_LS_TRIANGLE_SIZE / 2};
    Vector2 point6 = {BRIGHT_LS_RIGHT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y - BRIGHT_LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point6, point5, point4, Fade(Var_GetColor2(), Var_GetAlphaSelect()));
    //Draw percentage
    char percentTxt[5];
    int brightPercent = (int)((MIN_BRIGHTNESS - Var_GetBright()) / (MIN_BRIGHTNESS / 100.0f));
    snprintf(percentTxt, sizeof(percentTxt), "%d%%", brightPercent);
    Vector2 brightnessPercent_Size = MeasureTextEx(Var_GetFontBold(), percentTxt, BRIGHTNESS_PERCENT_SIZE, UIPAUSE_TITLE_SPACE);
    Vector2 brightnessPercent = {BRIGHTNESS_PERCENT_X, BRIGHTNESS_PERCENT_Y};
    DrawTextEx(Var_GetFontBold(), percentTxt, brightnessPercent, BRIGHTNESS_PERCENT_SIZE, UIPAUSE_TITLE_SPACE, Var_GetColor3());

}

//Brightness options init function
void Brightness_Init() {
    //Start not changing the brightness
    currentBrightnessState = NEITHER;
}

//Brightness options tick function
void Brightness_Tick() {
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
            }
            break;

        //Increasing brightness
        case RIGHT_PRESS:
            //If no longer pressing to the right, stop changing brightness
            if (!IsKeyDown(KEY_RIGHT) && !LS_RIGHT) {
                currentBrightnessState = NEITHER;
            }
            break;
    }

    //Action
    switch (currentBrightnessState) {
        //Not increasing or decreasing brightness
        case NEITHER:
            //Draw brightness options screen
            Brightness_Draw();
            //If brightness too high, set it at max
            if (Var_GetBright() < MAX_BRIGHTNESS) {
                Var_SetBright(MAX_BRIGHTNESS);
            }
            //If brightness too low, set it at min
            else if (Var_GetBright() > MIN_BRIGHTNESS) {
                Var_SetBright(MIN_BRIGHTNESS);
                Var_SetBrightX(BRIGHTNESS_LINE_X_START);
            }
            break;

        //Decreasing brightness
        case LEFT_PRESS:
            //Draw brightness options screen
            Brightness_Draw();
            //Decrease brightness
            Var_AddBright(BRIGHTNESS_INCREMENT);
            //Move brightness circle to the left
            Var_AddBrightX(- BRIGHTNESS_CIRCLE_INCREMENT);
            //If brightness too low, set it at min
            if (Var_GetBright() >= MIN_BRIGHTNESS) {
                Var_SetBright(MIN_BRIGHTNESS);
                Var_SetBrightX(BRIGHTNESS_LINE_X_START);
            }
            //Update the UI file
            Var_UpdateUITxt();
            break;

        //Increasing brightness
        case RIGHT_PRESS:
            //Draw brightness options screen
            Brightness_Draw();
            //Increase brightness
            Var_AddBright(- BRIGHTNESS_INCREMENT);
            //Move brightness circle to the right
            Var_AddBrightX(BRIGHTNESS_CIRCLE_INCREMENT);
            //If brightness too high, set it at max
            if (Var_GetBright() <= MAX_BRIGHTNESS) {
                Var_SetBright(MAX_BRIGHTNESS);
                Var_SetBrightX(BRIGHTNESS_CIRCLE_X);
            }
            //Update the UI file
            Var_UpdateUITxt();
            break;
    }
}
