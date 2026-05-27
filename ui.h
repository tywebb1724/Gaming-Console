#ifndef UI_H
#define UI_H
#include "games.h"

#define BACKGROUND_CLR (Color){ 30, 32, 36, 255 }

#define LEFT2 (LEFT1 - (SIDE2_IMG_H * (3 / 4.0f)))
#define LEFT1 (CENTER - (SIDE1_IMG_H * (3 / 4.0f)))
#define CENTER 400
#define RIGHT1 (CENTER + (SIDE1_IMG_H * (3 / 4.0f)))
#define RIGHT2 (RIGHT1 + (SIDE2_IMG_H * (3 / 4.0f)))

#define BOOT_TEXT_SIZE 

#define COMPANY_NAME "SP1DER GAMES"
#define COMPANY_TEXT_SIZE 20
#define COMPANY_Y 20
#define PICK_GAME_TEXT "PICK A GAME"
#define PICK_GAME_TEXT_SIZE 15
#define PICK_GAME_Y 40

#define GAMES_ON_SCREEN 5

#define CENTER_IMG_Y 100
#define CENTER_IMG_W 220.0f
#define CENTER_IMG_H ((4.0f/3) * CENTER_IMG_W)
#define CENTER_IMG_X (CENTER - (CENTER_IMG_W / 2))
#define IMG_Y_CENTER (CENTER_IMG_Y + (CENTER_IMG_H / 2))

#define THICKNESS_SELECT 4.0f
#define THICKNESS_OTHER 2.0f

#define SIDE1_IMG_W (CENTER_IMG_W * (3/4.0f))
#define SIDE1_IMG_H ((4.0f/3) * SIDE1_IMG_W)
#define SIDE1_IMG_Y (IMG_Y_CENTER - (SIDE1_IMG_H / 2))
#define LEFT1_IMG_X (LEFT1 - (SIDE1_IMG_W / 2))
#define RIGHT1_IMG_X (RIGHT1 - (SIDE1_IMG_W / 2))

#define SIDE2_IMG_W (CENTER_IMG_W * (1/2.0f))
#define SIDE2_IMG_H ((4.0f/3) * SIDE2_IMG_W)
#define SIDE2_IMG_Y (IMG_Y_CENTER - (SIDE2_IMG_H / 2))
#define LEFT2_IMG_X (LEFT2 - (SIDE2_IMG_W / 2))
#define RIGHT2_IMG_X (RIGHT2 - (SIDE2_IMG_W / 2))

#define SELECT_TEXT_SIZE 15
#define SELECT_PADDING 10
#define SELECT_TEXT1_X 325
#define SELECT_Y (CENTER_IMG_Y + CENTER_IMG_H + SELECT_PADDING)
#define SELECT_TEXT1 "PRESS"
#define A_BTN_X (SELECT_TEXT1_X + MeasureText(SELECT_TEXT1, SELECT_TEXT_SIZE) + SELECT_PADDING)
#define SELECT_TEXT2_X (A_BTN_X + BTN_SIZE + SELECT_PADDING)
#define SELECT_TEXT2 "/"
#define X_BTN_X (SELECT_TEXT2_X + MeasureText(SELECT_TEXT2, SELECT_TEXT_SIZE) + SELECT_PADDING)
#define SELECT_TEXT3_X (X_BTN_X + BTN_SIZE + SELECT_PADDING)
#define SELECT_TEXT3 "TO PLAY"
#define BTN_SIZE 10

void UI_LoadTextures();
int CenterText(char *text, int fontSize, int position);
void DrawImage();
void DrawGames();
void DrawSelect();
//Function for drawing the boot up screen
void UI_DrawBootScreen(void);
void UI_DrawMainMenu(int currentSelection);
void UI_DrawDiagnostics(void);

#endif