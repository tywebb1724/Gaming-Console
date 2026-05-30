#ifndef UI_H
#define UI_H
#include "games.h"

#define BACKGROUND_CLR (Color){ 30, 32, 36, 255 }
#define SCREEN_W 800
#define SCREEN_H 450
#define FPS 60

#define LEFT2 (LEFT1 - (SIDE2_IMG_H * (3 / 4.0f)))
#define LEFT1 (CENTER - (SIDE1_IMG_H * (3 / 4.0f)))
#define CENTER (SCREEN_W / 2)
#define RIGHT1 (CENTER + (SIDE1_IMG_H * (3 / 4.0f)))
#define RIGHT2 (RIGHT1 + (SIDE2_IMG_H * (3 / 4.0f)))

#define RIGHT 1
#define LEFT 0

#define BOOT_TEXT_SIZE 

#define COMPANY_NAME "SP1DER GAMES"
#define COMPANY_TEXT_SIZE 20
#define COMPANY_Y 20
#define PICK_GAME_TEXT "PICK A GAME"
#define PICK_GAME_TEXT_SIZE 15
#define PICK_GAME_Y 40

#define CENTER_IMG_Y 100
#define CENTER_IMG_W 220.0f
#define CENTER_IMG_H ((4.0f/3) * CENTER_IMG_W)
#define CENTER_IMG_X (CENTER - (CENTER_IMG_W / 2))
#define IMG_Y_CENTER (CENTER_IMG_Y + (CENTER_IMG_H / 2))

#define THICKNESS_SELECT_GAME 4.0f
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

#define SIDE3_IMG_W (CENTER_IMG_W * (1/4.0f))
#define SIDE3_IMG_H ((4.0f/3) * SIDE3_IMG_W)
#define SIDE3_IMG_Y (IMG_Y_CENTER - (SIDE3_IMG_H / 2))
#define LEFT3_IMG_X LEFT1_IMG_X
#define RIGHT3_IMG_X RIGHT1_IMG_X

#define SELECT_LEN (MeasureText(SELECT_TXT1, SELECT_TXT_SIZE) + MeasureText(SELECT_TXT2, SELECT_TXT_SIZE) + MeasureText(SELECT_TXT3, SELECT_TXT_SIZE) + (4 * BTN_RADIUS) + (4 * SELECT_PADDING_X))
#define SELECT_PADDING_Y 20.0f
#define SELECT_PADDING_X 5.0f
#define SELECT_TXT_SIZE 10.0f
#define SELECT_TXT1 "PRESS"
#define SELECT_TXT1_X (CENTER - (SELECT_LEN / 2))
#define SELECT_TXT_Y (CENTER_IMG_Y + CENTER_IMG_H + SELECT_PADDING_Y)
#define SELECT_TXT2 "/"
#define SELECT_TXT2_X (XBOX_A_X + BTN_RADIUS + SELECT_PADDING_X)
#define SELECT_TXT3 "TO PLAY"
#define SELECT_TXT3_X (PS_X_X + BTN_RADIUS + SELECT_PADDING_X)

#define BTN_RADIUS 8.0f
#define XBOX_A_X (SELECT_TXT1_X + MeasureText(SELECT_TXT1, SELECT_TXT_SIZE) + SELECT_PADDING_X + BTN_RADIUS)
#define PS_X_X (SELECT_TXT2_X + MeasureText(SELECT_TXT2, SELECT_TXT_SIZE) + SELECT_PADDING_X + BTN_RADIUS)
#define BTN_A_X_Y (SELECT_TXT_Y + SELECT_TXT_SIZE / 2)
#define XBOX_A_TXT_X (XBOX_A_X - MeasureText("A", SELECT_TXT_SIZE) / 2)
#define PS_X_TXT_X (PS_X_X - MeasureText("X", SELECT_TXT_SIZE) / 2)
#define THICKNESS_SELECT 2.0f

#define LEFT_ARROW_X 700
#define RIGHT_ARROW_X 725
#define ARROW_Y 50
#define MIDDLE_ARROW_X ((LEFT_ARROW_X + RIGHT_ARROW_X) / 2)
#define ARROW_TXT "SCROLL GAMES"
#define ARROW_TXT_X (MIDDLE_ARROW_X - (MeasureText(ARROW_TXT, SELECT_TXT_SIZE) / 2))
#define ARROW_TXT_Y (ARROW_Y + SELECT_PADDING_Y)

#define SCROLL_THRESHOLD 3.0f


#define XBOX_B_X (MIDDLE_ARROW_X)
#define PS_O_X (XBOX_B_X + (2.0f * BTN_RADIUS) + SELECT_PADDING_X)
#define BTN_B_O_Y (BTN_B_O_TXT_Y + SELECT_TXT_SIZE / 2.0f)
#define BTN_B_O_TXT_Y (BACK_TXT_Y - SELECT_PADDING_Y)
#define XBOX_B_TXT_X (XBOX_B_X - MeasureText("B", SELECT_TXT_SIZE) / 2.0f)
#define PS_O_TXT_X (PS_O_X - MeasureText("O", SELECT_TXT_SIZE) / 2.0f)

#define BACK_SLASH_X


#define BACK_TXT "GO BACK"
#define BACK_TXT_X (MIDDLE_ARROW_X - MeasureText(BACK_TXT, SELECT_TXT_SIZE) / 2)
#define BACK_TXT_Y (ARROW_Y - 2 * BTN_RADIUS - SELECT_PADDING_Y / 2)




enum Scroll {
    SCROLL_NO,
    SCROLL_LEFT,
    SCROLL_RIGHT
};

extern enum Scroll scroll;

extern float currentX_L3;
extern float currentX_L2;
extern float currentX_L1;
extern float currentX_C;
extern float currentX_R1;
extern float currentX_R2;
extern float currentX_R3;

extern float currentY_L3;
extern float currentY_L2;
extern float currentY_L1;
extern float currentY_C;
extern float currentY_R1;
extern float currentY_R2;
extern float currentY_R3;

extern float currentW_L3;
extern float currentW_L2;
extern float currentW_L1;
extern float currentW_C;
extern float currentW_R1;
extern float currentW_R2;
extern float currentW_R3;

extern float currentH_L3;
extern float currentH_L2;
extern float currentH_L1;
extern float currentH_C;
extern float currentH_R1;
extern float currentH_R2;
extern float currentH_R3;

void UI_ChangeAlpa(float offRate, float onRate);
void UI_ResetDisplayCoords_Games();
void UI_DrawArrow(int xPos, int yPos, int direction);
void UI_DrawImage();
void UI_DrawGame();
void UI_DrawGames_Normal();
void UI_DrawGames_Left();
void UI_DrawGames_Right();
void UI_DrawScroll();
int UI_CenterText(char *text, int fontSize, int position);
void UI_DrawSelect();
//Function for drawing the boot up screen
void UI_DrawBootScreen(void);
void UI_DrawMainMenu();
void UI_DrawDiagnostics(void);

#endif