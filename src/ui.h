#ifndef UI_H
#define UI_H
#include "ui_config.h"
#include "games.h"
#include "config.h"



typedef enum {
    LIST,
    BRIGHTNESS,
    THEME,
    DISPLAY_DIAGNOSTICS,
    VIEW_DIAGNOSTICS
} OptionsState;

typedef enum {
    RESTART,
    PAUSED_BRIGHTNESS,
    PAUSED_CONTROLS,
    PAUSED_DIAGNOSTICS,
    EXIT
} PausedState;


typedef enum {
    SCREEN_MAIN,
    SCREEN_LIST,
    SCREEN_BRIGHTNESS,
    SCREEN_THEME,
    SCREEN_DISPLAY_DIAG,
    SCREEN_VIEW_DIAG
} ScreenState;

typedef enum {
    NORMAL,
    SCROLL_GAMES,
    SCROLL_CATEGORIES,
    OPTIONS
} MainMenuState;

//Scrolling states
typedef enum {
    SCROLL_NO,
    SCROLL_LEFT,
    SCROLL_RIGHT
} ScrollState;

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
} BrightPressed;

typedef enum {
    DIAG_NORMAL,
    DIAG_OPTIONS
} DiagState;

//States of the console
typedef enum {
    STATE_BOOT,
    STATE_MAIN_MENU,
    STATE_APP_LAUNCHER,
    STATE_LIST,
    STATE_VIEW_DIAG
} ConsoleState;


GameState UI_DrawPaused_Menu(bool start);
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
void MainMenu_DrawDiagnostics();
//void Paused_DrawController(Game game);
void UI_DrawOptions();
ConsoleState MainMenu_Tick(ConsoleState currentConsoleState) ;
//Function for drawing the diagnostics screen
ConsoleState UI_DrawDiagnostics(ConsoleState currentConsoleState);

void UI_Init();



#endif