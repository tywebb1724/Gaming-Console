#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include "var.h"

//Drawing main text
#define BRIGHTNESS_TXT "Adjust brightness"
#define BRIGHTNESS_TXT_SIZE (Var_GetMonitorWidth() / 40.0f)
#define BRIGHTNESS_TXT_SPACE (Var_GetMonitorWidth() / 400.0f)
#define BRIGHTNESS_TXT_X (CENTER_X - brightnessSize.x / 2)
#define BRIGHTNESS_TXT_Y (UIPAUSE_RECT_Y + UIPAUSE_TXT_SPACING_Y - brightnessSize.y / 2)
//Drawing line and circle
#define BRIGHTNESS_LINE_X_START (UIPAUSE_RECT_X + UIPAUSE_TXT_SPACING_Y * 2)
#define BRIGHTNESS_LINE_X_END (UIPAUSE_RECT_X + UIPAUSE_RECT_W - UIPAUSE_TXT_SPACING_Y * 2)
#define BRIGHTNESS_LINE_Y (UIPAUSE_RECT_Y + UIPAUSE_RECT_H / 5 * 3)
#define BRIGHTNESS_LINE_THICK (Var_GetMonitorHeight() / 650.0f)
#define BRIGHTNESS_RADIUS (Var_GetMonitorWidth() / 90.0f)
#define BRIGHTNESS_PX_PER_UNIT ((BRIGHTNESS_LINE_X_END - BRIGHTNESS_LINE_X_START) / (float)MIN_BRIGHTNESS)
#define BRIGHTNESS_CIRCLE_X (BRIGHTNESS_LINE_X_END - Var_GetBright() * BRIGHTNESS_PX_PER_UNIT)
//Drawing arrows
#define BRIGHT_LS_LEFT_X (CENTER_X - Var_GetMonitorWidth() / 55.0f)
#define BRIGHT_LS_RIGHT_X (CENTER_X + Var_GetMonitorWidth() / 55.0f)
#define BRIGHT_LS_Y (BRIGHTNESS_LINE_Y + BRIGHT_LS_RADIUS + UIPAUSE_TXT_SPACING_Y)
#define BRIGHT_LS_TRIANGLE_SIZE (Var_GetMonitorWidth() / 120.0f)
#define BRIGHT_LS_RADIUS (Var_GetMonitorWidth() / 135.0f)
//Drawing percentage
#define BRIGHTNESS_PERCENT_SIZE (Var_GetMonitorWidth() / 60.0f)
#define BRIGHTNESS_PERCENT_X (CENTER_X - brightnessPercentSize.x / 2)
#define BRIGHTNESS_PERCENT_Y (BRIGHTNESS_LINE_Y - 3 * UIPAUSE_TXT_SPACING_Y / 2 - brightnessPercentSize.y / 2)
//Brightness increment
#define BRIGHTNESS_INCREMENT (MIN_BRIGHTNESS / 100.0f * GetFrameTime() * 60.0f)

//State of the brightness options
typedef enum {
    NEITHER,
    LEFT_PRESS,
    RIGHT_PRESS
} BrightnessState;

//Brightness options init function
void Brightness_Init();
//Brightness options tick function
void Brightness_Tick();


#endif