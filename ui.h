#ifndef UI_H
#define UI_H
#include "games.h"

enum Scroll {
    SCROLL_NO,
    SCROLL_LEFT,
    SCROLL_RIGHT
};

extern enum Scroll scrollGames;
extern enum Scroll scrollCategories;

//Change the alpha value for fading the display
void UI_ChangeAlpa(float offRate, float onRate);
//Reset the coordinates for the displayed games
void UI_ResetDisplayCoords_Games();
//Reset the coordinates for the displayed consoles
void UI_DrawArrow(int xPos, int yPos, int direction);
//Function for drawing an image
void UI_DrawImage();
//Function for drawing a game cover
void UI_DrawGame();
//Function for drawing the games when in a static position
void UI_DrawGames_Left();
//Function for drawing the games when scrolling to the right
void UI_DrawGames_Right();
//Function for drawing the animation of scrolling games
void UI_DrawScroll_Games();
//Function for drawing a console cover
float UI_CenterImg_X(float width, float position);
//Function for centering an image on a certain Y position
float UI_CenterImg_Y(float height, float position);
//Function to center text on a certain X position
float UI_CenterText_X(char *text, int fontSize, int position);
//Function for drawing the controls at the top left corner
void UI_DrawCtrls_L();
//Function for drawing the controls at the top right corner
void UI_DrawCtrls_R();
//Function for drawing the select controls at the bottom
void UI_DrawSelect();
//Function for drawing the boot up screen
void UI_DrawBootScreen();
//Function for drawing the heading at the top of the screen
void UI_DrawHeading();
//Function for drawing games depending on whether it is scrolling
void UI_DrawGames();
//Function for drawing consoles depending on whether it is scrolling
void UI_DrawConsoles();
//Function for drawing the main menu
void UI_DrawMainMenu();
//Function for drawing the diagnostics screen
void UI_DrawDiagnostics();

void UI_DrawCategories();

#endif