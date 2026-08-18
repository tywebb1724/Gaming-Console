#ifndef ERASE_H
#define ERASE_H

#include <stdbool.h>
#include "games.h"
#include "ui.h"

//Macros for navigating the menu
#define MAX_TIME_ELAPSED_ERASE 1000000
#define THRESHOLD_TIME_ELAPSED_ERASE 0.25f
//Macros for drawing the menu
#define THICKNESS_UI_ERASE (Var_GetMonitorHeight() / 500.0f)
#define UI_ERASE_RECT_X (CENTER_X - UI_ERASE_RECT_W / 2)
#define UI_ERASE_RECT_Y (Var_GetMonitorHeight() * (1.0f / 2) - UI_ERASE_RECT_H / 2)
#define UI_ERASE_RECT_W (Var_GetMonitorWidth() * (2.0f / 5))
#define UI_ERASE_RECT_H (Var_GetMonitorHeight() * (1.0f / 3))
#define UI_ERASE_ROUND 0.025f
#define UI_ERASE_SEGMENTS 15
#define UI_ERASE_TXT_SPACING_Y (Var_GetMonitorHeight() / 30.0f)
#define UI_ERASE_TITLE "ERASE SAVE DATA?"
#define UI_ERASE_TITLE_SIZE (Var_GetMonitorWidth() / 30.0f)
#define UI_ERASE_TITLE_SPACE (Var_GetMonitorWidth() / 400.0f)
#define UI_ERASE_TITLE_X (CENTER_X - titleSize.x / 2)
#define UI_ERASE_TITLE_Y (UI_ERASE_RECT_Y + UI_ERASE_RECT_W / 15.0f - titleSize.y / 2)
#define UI_ERASE_ERASE "ERASE DATA"
#define UI_ERASE_ERASE_SIZE (UI_ERASE_TITLE_SIZE * (3.0f / 4))
#define UI_ERASE_ERASE_X (UI_ERASE_ERASE_RECT_X + UI_ERASE_SELECT_RECT_W / 2.0f - eraseSize.x / 2.0f)
#define UI_ERASE_ERASE_Y (UI_ERASE_ERASE_RECT_Y + UI_ERASE_SELECT_RECT_H / 2.0f - eraseSize.y / 2.0f)
#define UI_ERASE_CANCEL "CANCEL"
#define UI_ERASE_CANCEL_SIZE (UI_ERASE_TITLE_SIZE * (3.0f / 4))
#define UI_ERASE_CANCEL_X (UI_ERASE_CANCEL_RECT_X + UI_ERASE_SELECT_RECT_W / 2.0f - cancelSize.x / 2)
#define UI_ERASE_CANCEL_Y (UI_ERASE_ERASE_Y)
#define UI_ERASE_ERASE_RECT_X (UI_ERASE_RECT_X + UI_ERASE_RECT_W / 15.0f)
#define UI_ERASE_CANCEL_RECT_X (UI_ERASE_RECT_X + UI_ERASE_RECT_W - UI_ERASE_SELECT_RECT_W - UI_ERASE_RECT_W / 15.0f)
#define UI_ERASE_SELECT_RECT_W (UI_ERASE_RECT_W / 5.0f * 2)
#define UI_ERASE_SELECT_RECT_H (UI_ERASE_RECT_H / 3.0f)
#define UI_ERASE_ERASE_RECT_Y (UI_ERASE_RECT_Y + UI_ERASE_RECT_H / 5.0f * 2)
#define UI_ERASE_CANCEL_RECT_Y (UI_ERASE_ERASE_RECT_Y)
#define UI_ERASE_ERASED_SIZE (Var_GetMonitorWidth() / 35.0f)
#define UI_ERASE_ERASED "GAME DATA ERASED!"
#define UI_ERASE_ERASED_X (UI_ERASE_RECT_X + UI_ERASE_RECT_W / 2.0f - erasedSize.x / 2.0f)
#define UI_ERASE_ERASED_Y (UI_ERASE_RECT_Y + UI_ERASE_RECT_H / 2.0f - erasedSize.y / 2.0f)
#define UI_ERASE_NOT_ERASED "NO GAME DATA AVAILABLE"
#define UI_ERASE_SELECT "[A/ENTER] SELECT"
#define UI_ERASE_SELECT_SIZE (UI_ERASE_TITLE_SIZE * (1.0f / 3))
#define UI_ERASE_SELECT_X (UI_ERASE_RECT_X + UI_ERASE_TXT_SPACING_Y / 2.0f)
#define UI_ERASE_SELECT_Y (UI_ERASE_RECT_Y + UI_ERASE_RECT_H - UI_ERASE_TXT_SPACING_Y / 2 - selectSize.y)
#define UI_ERASE_BACK "[B/ESC] BACK"
#define UI_ERASE_BACK_SIZE (UI_ERASE_TITLE_SIZE * (1.0f / 3))
#define UI_ERASE_BACK_X (UI_ERASE_RECT_X + UI_ERASE_RECT_W - UI_ERASE_TXT_SPACING_Y / 2.0f - backSize.x)
#define UI_ERASE_BACK_Y (UI_ERASE_RECT_Y + UI_ERASE_RECT_H - UI_ERASE_TXT_SPACING_Y / 2 - backSize.y)
#define UI_ERASE_CLOSE "[A/ENTER] CLOSE"
#define UI_ERASE_CLOSE_SIZE (UI_ERASE_TITLE_SIZE * (1.0f / 3))
#define UI_ERASE_CLOSE_X (UI_ERASE_RECT_X + UI_ERASE_RECT_W - UI_ERASE_TXT_SPACING_Y / 2.0f - closeSize.x)
#define UI_ERASE_CLOSE_Y (UI_ERASE_RECT_Y + UI_ERASE_RECT_H - UI_ERASE_TXT_SPACING_Y / 2 - closeSize.y)

//State of the erase state menu
typedef enum {
    ERASE,
    CANCEL,
    DONE
} EraseState;

//Erase game data init function
void Erase_Init();
//Erase game data tick function
void Erase_Tick(const game_t* game, UIState* currentUIState);

#endif