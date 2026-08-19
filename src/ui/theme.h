#ifndef THEME_H
#define THEME_H

//Macros for drawing the menu
#define THEME_TXT "Select theme"
#define THEME_TXT_SIZE (Var_GetMonitorWidth() / 40.0f)
#define THEME_TXT_SPACE (Var_GetMonitorWidth() / 400.0f)
#define THEME_TXT_X (CENTER_X - themeSize.x / 2)
#define THEME_TXT_Y (UIPAUSE_RECT_Y + UIPAUSE_TXT_SPACING_Y - themeSize.y / 2)
#define THEME_RECT_X (UIPAUSE_RECT_X + 2 * UIPAUSE_TXT_SPACING_Y)
#define THEME_RECT_W (UIPAUSE_RECT_W  - 4 * UIPAUSE_TXT_SPACING_Y)
#define THEME_RECT_H (themeSize.y)
#define THEME_RECT_1_Y (THEME_TXT_Y + themeSize.y + UIPAUSE_TXT_SPACING_Y / 2.0f)
#define THEME_X2 (THEME_RECT_X + THEME_RECT_W / 3)
#define THEME_X3 (THEME_X2 + THEME_RECT_W / 3)
#define THEME_RECT_2_Y (THEME_RECT_1_Y + THEME_RECT_H + UIPAUSE_TXT_SPACING_Y / 4.0f * 3)
#define THEME_RECT_3_Y (THEME_RECT_2_Y + THEME_RECT_H + UIPAUSE_TXT_SPACING_Y / 4.0f * 3)
#define THEME_RECT_4_Y (THEME_RECT_3_Y + THEME_RECT_H + UIPAUSE_TXT_SPACING_Y / 4.0f * 3)

//State of the theme options
typedef enum {
    THEME_1,
    THEME_2,
    THEME_3,
    THEME_4
} ThemeState;

//Theme options init function
void Theme_Init(void);
//Theme options tick function
void Theme_Tick(void);

#endif