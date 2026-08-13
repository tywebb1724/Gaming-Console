#include "erase.h"
#include "var.h"
#include "config.h"
#include "controller_config.h"

EraseState currentEraseState;
float eraseTimeElapsed;
bool erased;

//Update the time elapsed on current option
static void Erase_UpdateTime() {
    //Increment the time elapsed
    eraseTimeElapsed += GetFrameTime();
    //If time getting too big, bring back to the threshold
    if (eraseTimeElapsed > 1000000) {
        eraseTimeElapsed = 0.25f;
    }
}

//Draw erase save data menu
void Erase_DrawMenu(const game_t* game) {
    //Draw whole section
    Rectangle rectSec = {
        UI_ERASE_RECT_X,
        UI_ERASE_RECT_Y,
        UI_ERASE_RECT_W,
        UI_ERASE_RECT_H};
    DrawRectangleRounded(rectSec, UI_ERASE_ROUND, UI_ERASE_SEGMENTS, Var_GetColor2());
    DrawRectangleRoundedLinesEx(rectSec, UI_ERASE_ROUND, UI_ERASE_SEGMENTS, THICKNESS_UI_ERASE, Var_GetColor3());
    //Draw title
    Vector2 titleSize = MeasureTextEx(Var_GetFontBold(), UI_ERASE_TITLE, UI_ERASE_TITLE_SIZE, UI_ERASE_TITLE_SPACE);
    Vector2 title = {UI_ERASE_TITLE_X, UI_ERASE_TITLE_Y};
    DrawTextEx(Var_GetFontBold(), UI_ERASE_TITLE, title, UI_ERASE_TITLE_SIZE, UI_ERASE_TITLE_SPACE, Var_GetColor3());
    //Draw erase
    Vector2 eraseSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_ERASE, UI_ERASE_ERASE_SIZE, UI_ERASE_TITLE_SPACE);
    Vector2 erase = {UI_ERASE_ERASE_X, UI_ERASE_ERASE_Y};
    Rectangle rectErase = {
        UI_ERASE_ERASE_RECT_X,
        UI_ERASE_ERASE_RECT_Y,
        UI_ERASE_SELECT_RECT_W,
        UI_ERASE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectErase, UI_ERASE_ROUND, UI_ERASE_SEGMENTS, THICKNESS_UI_ERASE, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), UI_ERASE_ERASE, erase, UI_ERASE_ERASE_SIZE, UI_ERASE_TITLE_SPACE, Var_GetColor3());
    //Draw cancel
    Vector2 cancelSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_CANCEL, UI_ERASE_CANCEL_SIZE, UI_ERASE_TITLE_SPACE);
    Vector2 cancel = {UI_ERASE_CANCEL_X, UI_ERASE_CANCEL_Y};
    Rectangle rectCancel = {
        UI_ERASE_CANCEL_RECT_X,
        UI_ERASE_CANCEL_RECT_Y,
        UI_ERASE_SELECT_RECT_W,
        UI_ERASE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectCancel, UI_ERASE_ROUND, UI_ERASE_SEGMENTS, THICKNESS_UI_ERASE, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), UI_ERASE_CANCEL, cancel, UI_ERASE_CANCEL_SIZE, UI_ERASE_TITLE_SPACE, Var_GetColor3());

    if (currentEraseState == ERASE) {
        DrawRectangleRoundedLinesEx(rectErase, UI_ERASE_ROUND, UI_ERASE_SEGMENTS, 2 * THICKNESS_UI_ERASE, Var_GetColor1());
    }
    else if (currentEraseState == CANCEL) {
        DrawRectangleRoundedLinesEx(rectCancel, UI_ERASE_ROUND, UI_ERASE_SEGMENTS, 2 * THICKNESS_UI_ERASE, Var_GetColor1());
    }
    //Draw select text
    Vector2 selectSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_SELECT, UI_ERASE_SELECT_SIZE, UI_ERASE_TITLE_SPACE);
    Vector2 select = {UI_ERASE_SELECT_X, UI_ERASE_SELECT_Y};
    DrawTextEx(Var_GetFontRegular(), UI_ERASE_SELECT, select, UI_ERASE_SELECT_SIZE, UI_ERASE_TITLE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    //Draw back text
    Vector2 backSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_BACK, UI_ERASE_BACK_SIZE, UI_ERASE_TITLE_SPACE);
    Vector2 back = {UI_ERASE_BACK_X, UI_ERASE_BACK_Y};
    DrawTextEx(Var_GetFontRegular(), UI_ERASE_BACK, back, UI_ERASE_BACK_SIZE, UI_ERASE_TITLE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
}

//Draw done erased menu
void Erase_DrawDone(const game_t* game) {
    //Draw whole section
    Rectangle rectSec = {
        UI_ERASE_RECT_X,
        UI_ERASE_RECT_Y,
        UI_ERASE_RECT_W,
        UI_ERASE_RECT_H};
    DrawRectangleRounded(rectSec, UI_ERASE_ROUND, UI_ERASE_SEGMENTS, Var_GetColor2());
    DrawRectangleRoundedLinesEx(rectSec, UI_ERASE_ROUND, UI_ERASE_SEGMENTS, THICKNESS_UI_ERASE, Var_GetColor3());
    
    //Draw erased
    if (erased) {
        Vector2 erasedSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_ERASED, UI_ERASE_ERASED_SIZE, UI_ERASE_TITLE_SPACE);
        Vector2 erased = {UI_ERASE_ERASED_X, UI_ERASE_ERASED_Y};
        DrawTextEx(Var_GetFontRegular(), UI_ERASE_ERASED, erased, UI_ERASE_ERASED_SIZE, UI_ERASE_TITLE_SPACE, Var_GetColor3());
    }
    else {
        Vector2 erasedSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_NOT_ERASED, UI_ERASE_ERASED_SIZE, UI_ERASE_TITLE_SPACE);
        Vector2 erased = {UI_ERASE_ERASED_X, UI_ERASE_ERASED_Y};
        DrawTextEx(Var_GetFontRegular(), UI_ERASE_NOT_ERASED, erased, UI_ERASE_ERASED_SIZE, UI_ERASE_TITLE_SPACE, Var_GetColor3());
    }
    //Draw close text
    Vector2 closeSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_CLOSE, UI_ERASE_CLOSE_SIZE, UI_ERASE_TITLE_SPACE);
    Vector2 close = {UI_ERASE_CLOSE_X, UI_ERASE_CLOSE_Y};
    DrawTextEx(Var_GetFontRegular(), UI_ERASE_CLOSE, close, UI_ERASE_CLOSE_SIZE, UI_ERASE_TITLE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
}

void Erase_Init() {
    currentEraseState = ERASE;
    eraseTimeElapsed = 0.0f;
}

void Erase_Tick(const game_t* game, UIState* currentUIState) {
    //Transition
    switch (currentEraseState) {
        case ERASE:
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || 
               ((LS_LEFT || LS_RIGHT || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) && eraseTimeElapsed >= 0.2f)) {
                currentEraseState = CANCEL;
                eraseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Controller_SetWasPressed_A(true);
                erased = Games_ClearData(game);
                currentEraseState = DONE;
            }
            break;

        case CANCEL:
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || 
               ((LS_LEFT || LS_RIGHT || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) && eraseTimeElapsed >= 0.2f)) {
                currentEraseState = ERASE;
                eraseTimeElapsed = 0.0f;
            }
            break;
        case DONE:
            break;
    }

    //Action
    switch (currentEraseState) {
        case ERASE:
            if ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Controller_GetWasPressed_B()) {
                Controller_SetWasPressed_B(true);
                *currentUIState = NORMAL;
            }
            Erase_DrawMenu(game);
            Erase_UpdateTime();
            break;

        case CANCEL:
            if ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Controller_GetWasPressed_B()) {
                Controller_SetWasPressed_B(true);
                *currentUIState = NORMAL;
            }
            else if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Controller_SetWasPressed_A(true);
                *currentUIState = NORMAL;
            }
            Erase_DrawMenu(game);
            Erase_UpdateTime();
            break;

        case DONE:
            if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Controller_SetWasPressed_A(true);
                *currentUIState = NORMAL;
            }
            Erase_DrawDone(game);
            break;
    }

}