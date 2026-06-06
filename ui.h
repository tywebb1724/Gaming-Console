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
//Function for drawing an image
void UI_DrawImage();
//Function for drawing a console cover
float UI_CenterImg_X(float width, float position);
//Function for centering an image on a certain Y position
float UI_CenterImg_Y(float height, float position);
//Function to center text on a certain X position
float UI_CenterText_X(char *text, int fontSize, int position);
//Function for drawing the boot up screen
void UI_DrawBootScreen();
//Function for drawing the main menu
void UI_DrawMainMenu();
//Function for drawing the diagnostics screen
void UI_DrawDiagnostics();


#endif