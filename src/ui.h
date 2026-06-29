#ifndef UI_H
#define UI_H
#include "games.h"


typedef enum {
    LIST,
    BRIGHTNESS,
    THEME,
    DISPLAY_DIAGNOSTICS,
    VIEW_DIAGNOSTICS
} optionsState;

typedef enum {
    SCREEN_MAIN,
    SCREEN_LIST,
    SCREEN_BRIGHTNESS,
    SCREEN_THEME,
    SCREEN_DISPLAY_DIAG,
    SCREEN_VIEW_DIAG
} screenState;

typedef enum {
    NORMAL,
    SCROLL_GAMES,
    SCROLL_CATEGORIES,
    OPTIONS
} mainMenuState;

//Scrolling states
typedef enum {
    SCROLL_NO,
    SCROLL_LEFT,
    SCROLL_RIGHT
} Scroll;

typedef enum {
    THEME_1,
    THEME_2,
    THEME_3,
    THEME_4
} theme_t;

typedef enum {
    NEITHER,
    LEFT_PRESS,
    RIGHT_PRESS
} brightPressed;

typedef enum {
    DIAG_NORMAL,
    DIAG_OPTIONS
} diagState;

//States of the console
typedef enum {
    STATE_BOOT,
    STATE_MAIN_MENU,
    STATE_APP_LAUNCHER,
    STATE_LIST,
    STATE_VIEW_DIAG
} ConsoleState;

//Types of font
extern Font fontRegular;
extern Font fontBold;
//Variables to show whether user is scrolling
extern Scroll scrollGames;
extern Scroll scrollCategories;
//Different alphas
extern float alphaCategories_Out;
extern float alphaCategories_In;

extern float brightness;
extern float brightCircleX;

extern Color themeColor1;
extern Color themeColor2;
extern Color themeColor3;
extern theme_t currentTheme;
extern bool displayDiag;
extern bool mouseWasPressed;

//Reset the coordinates for the displayed games after games scroll
void UI_ResetDisplayCoords_Games();
//Reset the coordinates for the displayed games after category scroll
void UI_ResetDisplayCoords_Scroll();
//Function for drawing an image
void UI_DrawImage(float alpha);
//Function for drawing a console cover
float UI_CenterImg_X(float width, float position);
//Function for centering an image on a certain Y position
float UI_CenterImg_Y(float height, float position);
//Function to center text on a certain X position
float UI_CenterText_X(char *text, int fontSize, int position);
void UI_DrawOptionsBtn();
//Function for drawing the boot up screen
void UI_DrawBootScreen();

void UI_DrawOptions();
ConsoleState MainMenu_Tick(ConsoleState currentConsoleState) ;
//Function for drawing the diagnostics screen
ConsoleState UI_DrawDiagnostics(ConsoleState currentConsoleState);

#endif