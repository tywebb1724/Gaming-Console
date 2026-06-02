#ifndef UI_H
#define UI_H
#include "games.h"


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

void UI_ChangeAlpa(float offRate, float onRate);
void UI_ResetDisplayCoords_Games();
void UI_DrawArrow(int xPos, int yPos, int direction);
void UI_DrawImage();
void UI_DrawGame();
void UI_DrawGames_Normal();
void UI_DrawGames_Left();
void UI_DrawGames_Right();
void UI_DrawScroll();
float UI_CenterImg_X(float width, float position);
float UI_CenterImg_Y(float height, float position);
float UI_CenterText_X(char *text, int fontSize, int position);
void UI_DrawSelect();
//Function for drawing the boot up screen
void UI_DrawBootScreen(void);
void UI_DrawMainMenu();
void UI_DrawDiagnostics(void);

#endif