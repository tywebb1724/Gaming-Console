#include "erase.h"
#include "var.h"
#include "config.h"
#include "controller.h"

//State of the erase game data menu
static EraseState currentEraseState;
//Time on selected section
static float eraseTimeElapsed;
//Whether game data successfully erased
static bool erased;

//Update the time elapsed on current option
static void Erase_UpdateTime(void) {
    eraseTimeElapsed += GetFrameTime();
    //If time getting too big, bring back to the threshold
    if (eraseTimeElapsed > MAX_TIME_ELAPSED) {
        eraseTimeElapsed = THRESHOLD_TIME_ELAPSED;
    }
}

//Draw erase save data menu
static void Erase_DrawMenu(void) {
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
    //Draw outline on the selected section
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
static void Erase_DrawDone(void) {
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
        Vector2 erasedPos = {UI_ERASE_ERASED_X, UI_ERASE_ERASED_Y};
        DrawTextEx(Var_GetFontRegular(), UI_ERASE_ERASED, erasedPos, UI_ERASE_ERASED_SIZE, UI_ERASE_TITLE_SPACE, Var_GetColor3());
    }
    else {
        Vector2 erasedSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_NOT_ERASED, UI_ERASE_ERASED_SIZE, UI_ERASE_TITLE_SPACE);
        Vector2 erasedPos = {UI_ERASE_ERASED_X, UI_ERASE_ERASED_Y};
        DrawTextEx(Var_GetFontRegular(), UI_ERASE_NOT_ERASED, erasedPos, UI_ERASE_ERASED_SIZE, UI_ERASE_TITLE_SPACE, Var_GetColor3());
    }
    //Draw close text
    Vector2 closeSize = MeasureTextEx(Var_GetFontRegular(), UI_ERASE_CLOSE, UI_ERASE_CLOSE_SIZE, UI_ERASE_TITLE_SPACE);
    Vector2 close = {UI_ERASE_CLOSE_X, UI_ERASE_CLOSE_Y};
    DrawTextEx(Var_GetFontRegular(), UI_ERASE_CLOSE, close, UI_ERASE_CLOSE_SIZE, UI_ERASE_TITLE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
}

//Inputs pressed to scroll to other section
static bool Erase_Scroll(void) {
    return IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || 
            ((LS_LEFT || LS_RIGHT || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) && eraseTimeElapsed >= SELECT_TIME);
}

//Inputs to select
static bool Erase_Select(void) {
    return (IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A();
}

//Inputs to go back
static bool Erase_Back(void) {
    return (IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Controller_GetWasPressed_B();
}

//Erase game data init function
void Erase_Init(void) {
    currentEraseState = ERASE;
    eraseTimeElapsed = 0.0f;
    erased = false;
}

//Erase game data tick function
void Erase_Tick(const game_t* game, UIState* currentUIState) {
    //Transition
    switch (currentEraseState) {
        //Erase data selected
        case ERASE:
            //Switch to other section of left or right is pressed
            if (Erase_Scroll()) {
                currentEraseState = CANCEL;
                eraseTimeElapsed = 0.0f;
            }
            //Erase data if section is pressed
            else if (Erase_Select()) {
                Controller_SetWasPressed_A(true);
                erased = Games_ClearData(game);
                currentEraseState = DONE;
            }
            break;

        //Cancel erase selected
        case CANCEL:
            //Switch to other section of left or right is pressed
            if (Erase_Scroll()) {
                currentEraseState = ERASE;
                eraseTimeElapsed = 0.0f;
            }
            break;
        
        //Done erasing data
        case DONE:
            break;
    }

    //Action
    switch (currentEraseState) {
        //Erase data selected
        case ERASE:
            //Exit menu if back is pressed
            if (Erase_Back()) {
                Controller_SetWasPressed_B(true);
                *currentUIState = NORMAL;
            }
            Erase_DrawMenu();
            Erase_UpdateTime();
            break;

        //Cancel erase selected
        case CANCEL:
            //Exit menu if back is pressed or section is selected
            if (Erase_Back()) {
                Controller_SetWasPressed_B(true);
                *currentUIState = NORMAL;
            }
            else if (Erase_Select()) {
                Controller_SetWasPressed_A(true);
                *currentUIState = NORMAL;
            }
            Erase_DrawMenu();
            Erase_UpdateTime();
            break;

        //Done erasing data
        case DONE:
            //Exit menu if pressed
            if (Erase_Select()) {
                Controller_SetWasPressed_A(true);
                *currentUIState = NORMAL;
            }
            Erase_DrawDone();
            break;
    }

}