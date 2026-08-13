#include "playpause.h"
#include "controlsdisplay_config.h"
#include "playpause_config.h"
#include "controller_config.h"
#include "raylib.h"
#include "config.h"
#include "games.h"
#include "var.h"
#include <stdio.h>
#include <string.h>
#include "ui/ui.h"

//Current state of the pause screen
static PlayPauseState currentPlayPauseState;
//Controller image
static Texture2D controlsImg;
static Texture2D keyImg;
static Texture2D mouseImg;
static Texture2D arrowsImg;
//Variable to keep track of if controls are being displayed
static bool displayControls;
//Time spent on current section
static float playPauseTimeElapsed;

//Update the time elapsed on one section of the paused screen
static void PlayPause_UpdateTime() {
    playPauseTimeElapsed += GetFrameTime();
    //If the variable is getting too big, bring it back down
    if (playPauseTimeElapsed > 1000000) {
        playPauseTimeElapsed = 0.25f;
    }
}

//Draw the controls for the game
static void PlayPause_DrawControls(const game_t* game) {
    //Draw controller image
    Rectangle sourceRect_Controller = {0.0f, 0.0f, (float)controlsImg.width, (float)controlsImg.height};
    Rectangle destRect_Controller = {PLAYPAUSE_CONTROLLER_X, PLAYPAUSE_CONTROLLER_Y, PLAYPAUSE_CONTROLLER_W, PLAYPAUSE_CONTROLLER_H};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(controlsImg, sourceRect_Controller, destRect_Controller, origin, 0.0f, WHITE);
    //Draw keyboard rectangle
    Rectangle rect = {PLAYPAUSE_KEYBOARD_X, PLAYPAUSE_KEYBOARD_Y, PLAYPAUSE_KEYBOARD_W, PLAYPAUSE_KEYBOARD_H};
    DrawRectanglePro(rect, origin, 0.0f, WHITE);
    Vector2 middleLineStart = {PLAYPAUSE_KEYBOARD_X, PLAYPAUSE_CONTROLLER_Y};
    Vector2 middleLineEnd = {PLAYPAUSE_KEYBOARD_X, PLAYPAUSE_CONTROLLER_Y + PLAYPAUSE_CONTROLLER_H};
    DrawLineEx(middleLineStart, middleLineEnd, PLAYPAUSE_CONTROL_OUTLINE_THICKNESS, GRAY);
    //Initialize the text variables for all the controls
    char rDown_Txt[10] = "", rUp_Txt[10] = "", rLeft_Txt[10] = "", rRight_Txt[10] = "";
    char dPad_Txt[10] = "", rs_Txt[10] = "", back_Txt[10] = "", start_Txt[10] = "";
    char lt_Txt[10] = "", rt_Txt[10] = "", lb_Txt[15] = "", rb_Txt[15] = "", ls_Txt[10] = "";
    char home_Txt[10] = "PAUSE";
    char tab_Txt[10] = "", q_Txt[13] = "", w_Txt[10] = "", e_Txt[13] = "", u_Txt[10] = "", i_Txt[10] = "", o_Txt[10] = "";
    char a_Txt[10] = "", s_Txt[10] = "", d_Txt[10] = "", f_Txt[10] = "", j_Txt[10] = "", k_Txt[10] = "", l_Txt[10] = "";
    char enter_Txt[10] = "", lShift_Txt[10] = "", rShift_Txt[10] = "", space_Txt[10] = "", mouse_Txt[10] = "", lClick_Txt[10] = "", arrows_Txt[10] = "";
    //Draw circles and rectangles for backgrounds of each xbox control
    DrawCircle(PLAYPAUSE_RIGHT_FACE_DOWN_X, PLAYPAUSE_RIGHT_FACE_DOWN_Y, PLAYPAUSE_CONTROL_RADIUS, GRAY);
    DrawCircle(PLAYPAUSE_RIGHT_FACE_UP_X, PLAYPAUSE_RIGHT_FACE_UP_Y, PLAYPAUSE_CONTROL_RADIUS, GRAY);
    DrawCircle(PLAYPAUSE_RIGHT_FACE_LEFT_X, PLAYPAUSE_RIGHT_FACE_LEFT_Y, PLAYPAUSE_CONTROL_RADIUS, GRAY);
    DrawCircle(PLAYPAUSE_RIGHT_FACE_RIGHT_X, PLAYPAUSE_RIGHT_FACE_RIGHT_Y, PLAYPAUSE_CONTROL_RADIUS, GRAY);
    DrawRectangle(PLAYPAUSE_HOME_RECT_X, PLAYPAUSE_HOME_RECT_Y, PLAYPAUSE_HOME_RECT_W, PLAYPAUSE_HOME_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_DPAD_RECT_X, PLAYPAUSE_DPAD_RECT_Y, PLAYPAUSE_DPAD_RECT_W, PLAYPAUSE_DPAD_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_RS_RECT_X, PLAYPAUSE_RS_RECT_Y, PLAYPAUSE_RS_RECT_W, PLAYPAUSE_RS_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_BACK_RECT_X, PLAYPAUSE_BACK_RECT_Y, PLAYPAUSE_BACK_RECT_W, PLAYPAUSE_BACK_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_START_RECT_X, PLAYPAUSE_START_RECT_Y, PLAYPAUSE_START_RECT_W, PLAYPAUSE_START_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_LT_RECT_X, PLAYPAUSE_LT_RECT_Y, PLAYPAUSE_LT_RECT_W, PLAYPAUSE_LT_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_RT_RECT_X, PLAYPAUSE_RT_RECT_Y, PLAYPAUSE_RT_RECT_W, PLAYPAUSE_RT_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_LB_RECT_X, PLAYPAUSE_LB_RECT_Y, PLAYPAUSE_LB_RECT_W, PLAYPAUSE_LB_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_RB_RECT_X, PLAYPAUSE_RB_RECT_Y, PLAYPAUSE_RB_RECT_W, PLAYPAUSE_RB_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_LS_RECT_X, PLAYPAUSE_LS_RECT_Y, PLAYPAUSE_LS_RECT_W, PLAYPAUSE_LS_RECT_H, GRAY);
    //Keyboard controls
    Rectangle sourceRect_Key = {0.0f, 0.0f, (float)keyImg.width, (float)keyImg.height};
    Rectangle destRect_Key = {PLAYPAUSE_KEY_X, PLAYPAUSE_KEY_Y, PLAYPAUSE_KEY_W, PLAYPAUSE_KEY_H};
    DrawTexturePro(keyImg, sourceRect_Key, destRect_Key, origin, 0.0f, WHITE);
    Rectangle sourceRect_Mouse = {0.0f, 0.0f, (float)mouseImg.width, (float)mouseImg.height};
    Rectangle destRect_Mouse = {PLAYPAUSE_MOUSE_X, PLAYPAUSE_MOUSE_Y, PLAYPAUSE_MOUSE_W, PLAYPAUSE_MOUSE_H};
    DrawTexturePro(mouseImg, sourceRect_Mouse, destRect_Mouse, origin, 0.0f, WHITE);
    Rectangle sourceRect_Arrow = {0.0f, 0.0f, (float)arrowsImg.width, (float)arrowsImg.height};
    Rectangle destRect_Arrow = {PLAYPAUSE_ARROWS_X, PLAYPAUSE_ARROWS_Y, PLAYPAUSE_ARROWS_W, PLAYPAUSE_ARROWS_H};
    DrawTexturePro(arrowsImg, sourceRect_Arrow, destRect_Arrow, origin, 0.0f, WHITE);
    //1st keyboard row
    Vector2 startTab = {PLAYPAUSE_KEY_TAB_LINE_X, PLAYPAUSE_KEY_TAB_LINE_Y_START};
    Vector2 endTab = {PLAYPAUSE_KEY_TAB_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP1};
    DrawLineEx(startTab, endTab, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectTab = {PLAYPAUSE_KEY_TAB_X, PLAYPAUSE_KEY_TAB_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectTab, origin, 0.0f, GRAY);
    Vector2 startQ = {PLAYPAUSE_KEY_Q_LINE_X, PLAYPAUSE_KEY_Q_LINE_Y_START};
    Vector2 endQ = {PLAYPAUSE_KEY_Q_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP2};
    DrawLineEx(startQ, endQ, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectQ = {PLAYPAUSE_KEY_Q_X, PLAYPAUSE_KEY_Q_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectQ, origin, 0.0f, GRAY);
    Vector2 startW = {PLAYPAUSE_KEY_W_LINE_X, PLAYPAUSE_KEY_W_LINE_Y_START};
    Vector2 endW = {PLAYPAUSE_KEY_W_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP1};
    DrawLineEx(startW, endW, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectW = {PLAYPAUSE_KEY_W_X, PLAYPAUSE_KEY_W_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectW, origin, 0.0f, GRAY);
    Vector2 startE = {PLAYPAUSE_KEY_E_LINE_X, PLAYPAUSE_KEY_E_LINE_Y_START};
    Vector2 endE = {PLAYPAUSE_KEY_E_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP2};
    DrawLineEx(startE, endE, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectE = {PLAYPAUSE_KEY_E_X, PLAYPAUSE_KEY_E_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectE, origin, 0.0f, GRAY);
    Vector2 startU = {PLAYPAUSE_KEY_U_LINE_X, PLAYPAUSE_KEY_U_LINE_Y_START};
    Vector2 endU = {PLAYPAUSE_KEY_U_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP1};
    DrawLineEx(startU, endU, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectU = {PLAYPAUSE_KEY_U_X, PLAYPAUSE_KEY_U_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectU, origin, 0.0f, GRAY);
    Vector2 startI = {PLAYPAUSE_KEY_I_LINE_X, PLAYPAUSE_KEY_I_LINE_Y_START};
    Vector2 endI = {PLAYPAUSE_KEY_I_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP2};
    DrawLineEx(startI, endI, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectI = {PLAYPAUSE_KEY_I_X, PLAYPAUSE_KEY_I_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectI, origin, 0.0f, GRAY);
    Vector2 startO = {PLAYPAUSE_KEY_O_LINE_X, PLAYPAUSE_KEY_O_LINE_Y_START};
    Vector2 endO = {PLAYPAUSE_KEY_O_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP1};
    DrawLineEx(startO, endO, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectO = {PLAYPAUSE_KEY_O_X, PLAYPAUSE_KEY_O_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectO, origin, 0.0f, GRAY);
    //2nd keyboard row
    Vector2 startA = {PLAYPAUSE_KEY_A_LINE_X, PLAYPAUSE_KEY_A_LINE_Y_START};
    Vector2 endA = {PLAYPAUSE_KEY_A_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startA, endA, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectA = {PLAYPAUSE_KEY_A_X, PLAYPAUSE_KEY_A_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectA, origin, 0.0f, GRAY);
    Vector2 startS = {PLAYPAUSE_KEY_S_LINE_X, PLAYPAUSE_KEY_S_LINE_Y_START};
    Vector2 endS = {PLAYPAUSE_KEY_S_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startS, endS, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectS = {PLAYPAUSE_KEY_S_X, PLAYPAUSE_KEY_S_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectS, origin, 0.0f, GRAY);
    Vector2 startD = {PLAYPAUSE_KEY_D_LINE_X, PLAYPAUSE_KEY_D_LINE_Y_START};
    Vector2 endD = {PLAYPAUSE_KEY_D_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startD, endD, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectD = {PLAYPAUSE_KEY_D_X, PLAYPAUSE_KEY_D_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectD, origin, 0.0f, GRAY);
    Vector2 startF = {PLAYPAUSE_KEY_F_LINE_X, PLAYPAUSE_KEY_F_LINE_Y_START};
    Vector2 endF = {PLAYPAUSE_KEY_F_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startF, endF, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectF = {PLAYPAUSE_KEY_F_X, PLAYPAUSE_KEY_F_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectF, origin, 0.0f, GRAY);
    Vector2 startJ = {PLAYPAUSE_KEY_J_LINE_X, PLAYPAUSE_KEY_J_LINE_Y_START};
    Vector2 endJ = {PLAYPAUSE_KEY_J_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startJ, endJ, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectJ = {PLAYPAUSE_KEY_J_X, PLAYPAUSE_KEY_J_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectJ, origin, 0.0f, GRAY);
    Vector2 startK = {PLAYPAUSE_KEY_K_LINE_X, PLAYPAUSE_KEY_K_LINE_Y_START};
    Vector2 endK = {PLAYPAUSE_KEY_K_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startK, endK, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectK = {PLAYPAUSE_KEY_K_X, PLAYPAUSE_KEY_K_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectK, origin, 0.0f, GRAY);
    Vector2 startL = {PLAYPAUSE_KEY_L_LINE_X, PLAYPAUSE_KEY_L_LINE_Y_START};
    Vector2 endL = {PLAYPAUSE_KEY_L_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startL, endL, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectL = {PLAYPAUSE_KEY_L_X, PLAYPAUSE_KEY_L_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectL, origin, 0.0f, GRAY);
    //Other keys
    Vector2 startEnter = {PLAYPAUSE_KEY_ENTER_LINE_X, PLAYPAUSE_KEY_ENTER_LINE_Y_START};
    Vector2 endEnter = {PLAYPAUSE_KEY_ENTER_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP1};
    DrawLineEx(startEnter, endEnter, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectEnter = {PLAYPAUSE_KEY_ENTER_X, PLAYPAUSE_KEY_ENTER_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectEnter, origin, 0.0f, GRAY);
    Vector2 startRShift = {PLAYPAUSE_KEY_RSHIFT_LINE_X, PLAYPAUSE_KEY_RSHIFT_LINE_Y_START};
    Vector2 endRShift = {PLAYPAUSE_KEY_RSHIFT_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startRShift, endRShift, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectRShift = {PLAYPAUSE_KEY_RSHIFT_X, PLAYPAUSE_KEY_RSHIFT_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectRShift, origin, 0.0f, GRAY);
    Vector2 startLShift = {PLAYPAUSE_KEY_LSHIFT_LINE_X, PLAYPAUSE_KEY_LSHIFT_LINE_Y_START};
    Vector2 endLShift = {PLAYPAUSE_KEY_LSHIFT_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startLShift, endLShift, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectLShift = {PLAYPAUSE_KEY_LSHIFT_X, PLAYPAUSE_KEY_LSHIFT_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectLShift, origin, 0.0f, GRAY);
    Vector2 startSpace = {PLAYPAUSE_KEY_SPACE_LINE_X, PLAYPAUSE_KEY_SPACE_LINE_Y_START};
    Vector2 endSpace = {PLAYPAUSE_KEY_SPACE_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startSpace, endSpace, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectSpace = {PLAYPAUSE_KEY_SPACE_X, PLAYPAUSE_KEY_SPACE_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectSpace, origin, 0.0f, GRAY);
    //Mouse
    Vector2 startMouse = {PLAYPAUSE_KEY_MOUSE_LINE_X, PLAYPAUSE_KEY_MOUSE_LINE_Y_START};
    Vector2 endMouse = {PLAYPAUSE_KEY_MOUSE_LINE_X, PLAYPAUSE_KEY_LINE_END_TOP1};
    DrawLineEx(startMouse, endMouse, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectMouse = {PLAYPAUSE_KEY_MOUSE_X, PLAYPAUSE_KEY_MOUSE_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectMouse, origin, 0.0f, GRAY);
    Vector2 startLClick = {PLAYPAUSE_KEY_LCLICK_LINE_X, PLAYPAUSE_KEY_LCLICK_LINE_Y_START};
    Vector2 endLClick = {PLAYPAUSE_KEY_LCLICK_LINE_X, PLAYPAUSE_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startLClick, endLClick, PLAYPAUSE_KEY_THICKNESS, GRAY);
    Rectangle rectLClick = {PLAYPAUSE_KEY_LCLICK_X, PLAYPAUSE_KEY_LCLICK_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectLClick, origin, 0.0f, GRAY);
    //Arrows
    Vector2 startArrows = {PLAYPAUSE_KEY_ARROWS_LINE_X1, PLAYPAUSE_KEY_ARROWS_LINE_Y};
    Vector2 endArrows = {PLAYPAUSE_KEY_ARROWS_LINE_X2, PLAYPAUSE_KEY_ARROWS_LINE_Y};
    DrawLineEx(startArrows, endArrows, PLAYPAUSE_ARROW_THICKNESS, GRAY);
    Rectangle rectArrows = {PLAYPAUSE_KEY_ARROWS_REC_X, PLAYPAUSE_KEY_ARROWS_REC_Y, PLAYPAUSE_KEY_REC_W, PLAYPAUSE_KEY_REC_H};
    DrawRectanglePro(rectArrows, origin, 0.0f, GRAY);
    //Set the text variables for the console and draw the non-shared ones
    if (strcmp(game->console, "Sega Genesis") == 0 || strcmp(game->console, "Sega CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(back_Txt,  sizeof(back_Txt),  "MODE");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "MODE");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "C");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rLeft = {PLAYPAUSE_RIGHT_FACE_LEFT_TXT_X, PLAYPAUSE_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "C");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 i = {PLAYPAUSE_KEY_I_TXT_X, PLAYPAUSE_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "X");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "X");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 u = {PLAYPAUSE_KEY_U_TXT_X, PLAYPAUSE_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "Y");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rUp = {PLAYPAUSE_RIGHT_FACE_UP_TXT_X, PLAYPAUSE_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(l_Txt,    sizeof(l_Txt),    "Y");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 l = {PLAYPAUSE_KEY_L_TXT_X, PLAYPAUSE_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "Z");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "Z");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 o = {PLAYPAUSE_KEY_O_TXT_X, PLAYPAUSE_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sega Game Gear") == 0 || strcmp(game->console, "Sega Master System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "1");
        snprintf(j_Txt,    sizeof(j_Txt),    "1");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "2");
        snprintf(k_Txt,    sizeof(k_Txt),    "2");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
    }
    else if (strcmp(game->console, "TurboGrafx-16") == 0 || strcmp(game->console, "TurboGrafx-CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "RUN");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "RUN");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "I");
        snprintf(k_Txt,    sizeof(k_Txt),    "I");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "II");
        snprintf(j_Txt,    sizeof(j_Txt),    "II");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Neo Geo Pocket Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "OPTION");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "OPTION");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "B");
        snprintf(k_Txt,    sizeof(k_Txt),    "B");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "A");
        snprintf(j_Txt,    sizeof(j_Txt),    "A");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "STICK");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "STICK");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "STICK");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "STICK");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
    }
    else if (strcmp(game->console, "Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Game Boy Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Atari Lynx") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Super Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "Y");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rLeft = {PLAYPAUSE_RIGHT_FACE_LEFT_TXT_X, PLAYPAUSE_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "Y");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 i = {PLAYPAUSE_KEY_I_TXT_X, PLAYPAUSE_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "X");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rUp = {PLAYPAUSE_RIGHT_FACE_UP_TXT_X, PLAYPAUSE_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(l_Txt,    sizeof(l_Txt),    "X");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 l = {PLAYPAUSE_KEY_L_TXT_X, PLAYPAUSE_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "L");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 u = {PLAYPAUSE_KEY_U_TXT_X, PLAYPAUSE_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "R");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 o = {PLAYPAUSE_KEY_O_TXT_X, PLAYPAUSE_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Game Boy Advance") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "L");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 u = {PLAYPAUSE_KEY_U_TXT_X, PLAYPAUSE_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "R");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 o = {PLAYPAUSE_KEY_O_TXT_X, PLAYPAUSE_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sony PlayStation") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "X");
        snprintf(k_Txt,    sizeof(k_Txt),    "X");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "O");
        snprintf(l_Txt,    sizeof(l_Txt),    "O");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 l = {PLAYPAUSE_KEY_L_TXT_X, PLAYPAUSE_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "SQR");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rLeft = {PLAYPAUSE_RIGHT_FACE_LEFT_TXT_X, PLAYPAUSE_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(j_Txt,    sizeof(j_Txt),    "SQR");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "TRI");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rUp = {PLAYPAUSE_RIGHT_FACE_UP_TXT_X, PLAYPAUSE_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "TRI");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 i = {PLAYPAUSE_KEY_I_TXT_X, PLAYPAUSE_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L1");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "L1");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 u = {PLAYPAUSE_KEY_U_TXT_X, PLAYPAUSE_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R1");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "R1");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 o = {PLAYPAUSE_KEY_O_TXT_X, PLAYPAUSE_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L2");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lt = {PLAYPAUSE_LT_TXT_X, PLAYPAUSE_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(q_Txt,    sizeof(q_Txt),    "L2");
        Vector2 q_Size = MeasureTextEx(Var_GetFontBold(), q_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 q = {PLAYPAUSE_KEY_Q_TXT_X, PLAYPAUSE_KEY_Q_TXT_Y};
        DrawTextEx(Var_GetFontBold(), q_Txt, q, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R2");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rt = {PLAYPAUSE_RT_TXT_X, PLAYPAUSE_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(e_Txt,    sizeof(e_Txt),    "R2");
        Vector2 e_Size = MeasureTextEx(Var_GetFontBold(), e_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 e = {PLAYPAUSE_KEY_E_TXT_X, PLAYPAUSE_KEY_E_TXT_Y};
        DrawTextEx(Var_GetFontBold(), e_Txt, e, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Nintendo 64") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(u_Txt,    sizeof(u_Txt),    "B");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 u = {PLAYPAUSE_KEY_U_TXT_X, PLAYPAUSE_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(o_Txt,    sizeof(o_Txt),    "A");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 o = {PLAYPAUSE_KEY_O_TXT_X, PLAYPAUSE_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "STICK");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "STICK");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "STICK");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "STICK");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(arrows_Txt,    sizeof(arrows_Txt),    "D-PAD");
        Vector2 arrows_Size = MeasureTextEx(Var_GetFontBold(), arrows_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 arrows = {PLAYPAUSE_KEY_ARROWS_TXT_X, PLAYPAUSE_KEY_ARROWS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), arrows_Txt, arrows, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(q_Txt,    sizeof(q_Txt),    "L");
        Vector2 q_Size = MeasureTextEx(Var_GetFontBold(), q_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 q = {PLAYPAUSE_KEY_Q_TXT_X, PLAYPAUSE_KEY_Q_TXT_Y};
        DrawTextEx(Var_GetFontBold(), q_Txt, q, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(e_Txt,    sizeof(e_Txt),    "R");
        Vector2 e_Size = MeasureTextEx(Var_GetFontBold(), e_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 e = {PLAYPAUSE_KEY_E_TXT_X, PLAYPAUSE_KEY_E_TXT_Y};
        DrawTextEx(Var_GetFontBold(), e_Txt, e, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "Z");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lt = {PLAYPAUSE_LT_TXT_X, PLAYPAUSE_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(space_Txt,    sizeof(space_Txt),    "Z");
        Vector2 space_Size = MeasureTextEx(Var_GetFontBold(), space_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 space = {PLAYPAUSE_KEY_SPACE_TXT_X, PLAYPAUSE_KEY_SPACE_TXT_Y};
        DrawTextEx(Var_GetFontBold(), space_Txt, space, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rs_Txt,    sizeof(rs_Txt),    "%s", "C");
        Vector2 rs_Size = MeasureTextEx(Var_GetFontBold(), rs_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rs = {PLAYPAUSE_RS_TXT_X, PLAYPAUSE_RS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rs_Txt, rs, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(j_Txt,    sizeof(j_Txt),    "C");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(k_Txt,    sizeof(k_Txt),    "C");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "C");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 i = {PLAYPAUSE_KEY_I_TXT_X, PLAYPAUSE_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(l_Txt,    sizeof(l_Txt),    "C");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 l = {PLAYPAUSE_KEY_L_TXT_X, PLAYPAUSE_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Arcade") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "1");
        snprintf(k_Txt,    sizeof(k_Txt),    "1");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 k = {PLAYPAUSE_KEY_K_TXT_X, PLAYPAUSE_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "2");
        snprintf(l_Txt,    sizeof(l_Txt),    "2");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 l = {PLAYPAUSE_KEY_L_TXT_X, PLAYPAUSE_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "STICK");
        snprintf(a_Txt,    sizeof(a_Txt),    "STICK");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "STICK");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "STICK");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "STICK");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "COIN");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "COIN");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rShift = {PLAYPAUSE_KEY_RSHIFT_TXT_X, PLAYPAUSE_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "3");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rLeft = {PLAYPAUSE_RIGHT_FACE_LEFT_TXT_X, PLAYPAUSE_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(j_Txt,    sizeof(j_Txt),    "3");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 j = {PLAYPAUSE_KEY_J_TXT_X, PLAYPAUSE_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "4");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rUp = {PLAYPAUSE_RIGHT_FACE_UP_TXT_X, PLAYPAUSE_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "4");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 i = {PLAYPAUSE_KEY_I_TXT_X, PLAYPAUSE_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "5");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "5");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 u = {PLAYPAUSE_KEY_U_TXT_X, PLAYPAUSE_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "6");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "6");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 o = {PLAYPAUSE_KEY_O_TXT_X, PLAYPAUSE_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "PC") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "ENTER");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "ENTER");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 enter = {PLAYPAUSE_KEY_ENTER_TXT_X, PLAYPAUSE_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt,sizeof(rDown_Txt),"%s", "USE");
        snprintf(space_Txt,    sizeof(space_Txt),    "USE");
        Vector2 space_Size = MeasureTextEx(Var_GetFontBold(), space_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 space = {PLAYPAUSE_KEY_SPACE_TXT_X, PLAYPAUSE_KEY_SPACE_TXT_Y};
        DrawTextEx(Var_GetFontBold(), space_Txt, space, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "MOVE/RUN");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE);
        Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "STRAFE");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE);
        Vector2 a = {PLAYPAUSE_KEY_A_TXT_X, PLAYPAUSE_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "MOVE");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE);
        Vector2 s = {PLAYPAUSE_KEY_S_TXT_X, PLAYPAUSE_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "STRAFE");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE);
        Vector2 d = {PLAYPAUSE_KEY_D_TXT_X, PLAYPAUSE_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "MOVE");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE);
        Vector2 w = {PLAYPAUSE_KEY_W_TXT_X, PLAYPAUSE_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, PLAYPAUSE_CONTROL_SIZE_DOOM1, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(lShift_Txt,    sizeof(lShift_Txt),    "RUN");
        Vector2 lShift_Size = MeasureTextEx(Var_GetFontBold(), lShift_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lShift = {PLAYPAUSE_KEY_LSHIFT_TXT_X, PLAYPAUSE_KEY_LSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lShift_Txt, lShift, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "MAP");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(tab_Txt,    sizeof(tab_Txt),    "MAP");
        Vector2 tab_Size = MeasureTextEx(Var_GetFontBold(), tab_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 tab = {PLAYPAUSE_KEY_TAB_TXT_X, PLAYPAUSE_KEY_TAB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), tab_Txt, tab, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rt_Txt, sizeof(rt_Txt), "%s", "FIRE");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rt = {PLAYPAUSE_RT_TXT_X, PLAYPAUSE_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lClick_Txt,    sizeof(lClick_Txt),    "FIRE");
        Vector2 lClick_Size = MeasureTextEx(Var_GetFontBold(), lClick_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lClick = {PLAYPAUSE_KEY_LCLICK_TXT_X, PLAYPAUSE_KEY_LCLICK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lClick_Txt, lClick, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(f_Txt,    sizeof(f_Txt),    "FIRE");
        Vector2 f_Size = MeasureTextEx(Var_GetFontBold(), f_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 f = {PLAYPAUSE_KEY_F_TXT_X, PLAYPAUSE_KEY_F_TXT_Y};
        DrawTextEx(Var_GetFontBold(), f_Txt, f, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "SWITCH GUNS");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM2, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE_DOOM2, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(q_Txt,    sizeof(q_Txt),    "SWITCH GUNS");
        Vector2 q_Size = MeasureTextEx(Var_GetFontBold(), q_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM2, PLAYPAUSE_CONTROL_SPACE);
        Vector2 q = {PLAYPAUSE_KEY_Q_TXT_X, PLAYPAUSE_KEY_Q_TXT_Y};
        DrawTextEx(Var_GetFontBold(), q_Txt, q, PLAYPAUSE_CONTROL_SIZE_DOOM2, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "SWITCH GUNS");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM2, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE_DOOM2, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(e_Txt,    sizeof(e_Txt),    "SWITCH GUNS");
        Vector2 e_Size = MeasureTextEx(Var_GetFontBold(), e_Txt, PLAYPAUSE_CONTROL_SIZE_DOOM2, PLAYPAUSE_CONTROL_SPACE);
        Vector2 e = {PLAYPAUSE_KEY_E_TXT_X, PLAYPAUSE_KEY_E_TXT_Y};
        DrawTextEx(Var_GetFontBold(), e_Txt, e, PLAYPAUSE_CONTROL_SIZE_DOOM2, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(rs_Txt,    sizeof(rs_Txt),    "%s", "TURN");
        Vector2 rs_Size = MeasureTextEx(Var_GetFontBold(), rs_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rs = {PLAYPAUSE_RS_TXT_X, PLAYPAUSE_RS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rs_Txt, rs, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
        snprintf(mouse_Txt,    sizeof(mouse_Txt),    "TURN");
        Vector2 mouse_Size = MeasureTextEx(Var_GetFontBold(), mouse_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 mouse = {PLAYPAUSE_KEY_MOUSE_TXT_X, PLAYPAUSE_KEY_MOUSE_TXT_Y};
        DrawTextEx(Var_GetFontBold(), mouse_Txt, mouse, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    //Controls all games use
    Vector2 rDown_Size = MeasureTextEx(Var_GetFontBold(), rDown_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 rDown = {PLAYPAUSE_RIGHT_FACE_DOWN_TXT_X, PLAYPAUSE_RIGHT_FACE_DOWN_TXT_Y};
    DrawTextEx(Var_GetFontBold(), rDown_Txt, rDown, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    Vector2 rRight_Size = MeasureTextEx(Var_GetFontBold(), rRight_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 rRight = {PLAYPAUSE_RIGHT_FACE_RIGHT_TXT_X, PLAYPAUSE_RIGHT_FACE_RIGHT_TXT_Y};
    DrawTextEx(Var_GetFontBold(), rRight_Txt, rRight, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    Vector2 home_Size = MeasureTextEx(Var_GetFontBold(), home_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 home = {PLAYPAUSE_HOME_TXT_X, PLAYPAUSE_HOME_TXT_Y};
    DrawTextEx(Var_GetFontBold(), home_Txt, home, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    Vector2 dPad_Size = MeasureTextEx(Var_GetFontBold(), dPad_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 dPad = {PLAYPAUSE_DPAD_TXT_X, PLAYPAUSE_DPAD_TXT_Y};
    DrawTextEx(Var_GetFontBold(), dPad_Txt, dPad, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    Vector2 start_Size = MeasureTextEx(Var_GetFontBold(), start_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 start = {PLAYPAUSE_START_TXT_X, PLAYPAUSE_START_TXT_Y};
    DrawTextEx(Var_GetFontBold(), start_Txt, start, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    //Draw back text
    Vector2 backSize = MeasureTextEx(Var_GetFontRegular(), PLAYPAUSE_CONTROL_BACK, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 back = {PLAYPAUSE_CONTROL_BACK_X, PLAYPAUSE_CONTROL_BACK_Y};
    DrawTextEx(Var_GetFontRegular(), PLAYPAUSE_CONTROL_BACK, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, Fade(Var_GetColor2(), Var_GetAlphaSelect()));
    //Draw outline
    Rectangle outline = {PLAYPAUSE_CONTROLLER_X, PLAYPAUSE_CONTROLLER_Y, PLAYPAUSE_CONTROLLER_W * 2, PLAYPAUSE_CONTROLLER_H};
    DrawRectangleLinesEx(outline, PLAYPAUSE_CONTROL_OUTLINE_THICKNESS, GRAY);
}

//Draw the pause screen
static void PlayPause_Draw() {
    //Draw whole section
    Rectangle rectSection = {
        PLAYPAUSE_RECT_X,
        PLAYPAUSE_RECT_Y,
        PLAYPAUSE_RECT_W,
        PLAYPAUSE_RECT_H};
    DrawRectangleRounded(rectSection, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, Var_GetColor2());
    DrawRectangleRoundedLinesEx(rectSection, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, Var_GetColor3());
    //Draw title
    Vector2 titleSize = MeasureTextEx(Var_GetFontBold(), PLAYPAUSE_TITLE, PLAYPAUSE_TITLE_SIZE, PLAYPAUSE_SPACE);
    Vector2 title = {PLAYPAUSE_TITLE_X, PLAYPAUSE_TITLE_Y};
    DrawTextEx(Var_GetFontBold(), PLAYPAUSE_TITLE, title, PLAYPAUSE_TITLE_SIZE, PLAYPAUSE_SPACE, Var_GetColor3());
    //Draw resume
    Vector2 resumeSize = MeasureTextEx(Var_GetFontRegular(), RESUME_TXT, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE);
    Vector2 resume = {PLAYPAUSE_X, RESUME_Y};
    Rectangle rectResume = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_RESUME_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectResume, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), RESUME_TXT, resume, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, Var_GetColor3());
    //Draw restart
    Vector2 restart = {PLAYPAUSE_X, RESTART_Y};
    Rectangle rectRestart = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_RESTART_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectRestart, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), RESTART_TXT, restart, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, Var_GetColor3());
    //Draw controls
    Vector2 controls = {PLAYPAUSE_X, CONTROLS_Y};
    Rectangle rectControls = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_CONTROLS_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectControls, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), CONTROLS_TXT, controls, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, Var_GetColor3());
    //Draw display/hide diagnostics
    Vector2 diagnostics = {PLAYPAUSE_X, DIAGNOSTICS_Y};
    Rectangle rectDiag = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_DIAGNOSTICS_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDiag, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, Var_GetColor3());
    char *diagTxt;
    if (Var_GetDiag()) {
        diagTxt = HIDE_DIAGNOSTICS_TXT;
    }
    else {
        diagTxt = DISPLAY_DIAGNOSTICS_TXT;
    }
    DrawTextEx(Var_GetFontRegular(), diagTxt, diagnostics, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, Var_GetColor3());
    //Draw exit
    Vector2 exit = {PLAYPAUSE_X, EXIT_Y};
    Rectangle rectExit = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_EXIT_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectExit, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, Var_GetColor3());
    DrawTextEx(Var_GetFontRegular(), EXIT_TXT, exit, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, Var_GetColor3());
   
    //Draw select
    Vector2 selectSize = MeasureTextEx(Var_GetFontRegular(), SELECT_TXT, SELECT_SIZE, PLAYPAUSE_SPACE);
    Vector2 select = {SELECT_X, SELECT_Y};
    DrawTextEx(Var_GetFontRegular(), SELECT_TXT, select, SELECT_SIZE, PLAYPAUSE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    //Draw back
    Vector2 backSize = MeasureTextEx(Var_GetFontRegular(), BACK_TXT, BACK_SIZE, PLAYPAUSE_SPACE);
    Vector2 back = {BACK_X, BACK_Y};
    DrawTextEx(Var_GetFontRegular(), BACK_TXT, back, BACK_SIZE, PLAYPAUSE_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    //Draw an outline around the selected section
    if (currentPlayPauseState == PLAYPAUSE_RESUME) {
        DrawRectangleRoundedLinesEx(rectResume, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
    else if (currentPlayPauseState == PLAYPAUSE_RESTART) {
        DrawRectangleRoundedLinesEx(rectRestart, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
    else if (currentPlayPauseState == PLAYPAUSE_CONTROLS) {
        DrawRectangleRoundedLinesEx(rectControls, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
    else if (currentPlayPauseState == PLAYPAUSE_DIAGNOSTICS) {
        DrawRectangleRoundedLinesEx(rectDiag, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
    else {
        DrawRectangleRoundedLinesEx(rectExit, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
}

//PlayPause initialization
void PlayPause_Init() {
    //Load images
    controlsImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/controller.png");
    keyImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/keyboard.png");
    mouseImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/mouse.png");
    arrowsImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/arrowkeys.png");
    //Time elapsed on selected section
    playPauseTimeElapsed = 0.0f;
    //Start pause menu at top section
    currentPlayPauseState = PLAYPAUSE_RESUME;
    //Don't display the controls
    displayControls = false;
}

//PlayPause tick
PlayState PlayPause_Tick() {
    // Transition
    switch (currentPlayPauseState) {
        //Resume game section
        case PLAYPAUSE_RESUME:
            //Move up or down a section
            if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_RESTART;
                playPauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_EXIT;
                playPauseTimeElapsed = 0.0f;
            }
            break;

        //Restart game section
        case PLAYPAUSE_RESTART:
            //Move up or down a section
            if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_CONTROLS;
                displayControls = false;
                playPauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_RESUME;
                playPauseTimeElapsed = 0.0f;
            }
            break;

        //View controls section
        case PLAYPAUSE_CONTROLS:
            //Move up or down a section
            if ((IsKeyPressed(KEY_DOWN) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && playPauseTimeElapsed >= 0.2f)) && !displayControls) {
                currentPlayPauseState = PLAYPAUSE_DIAGNOSTICS;
                playPauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP) || ((IsKeyPressed(KEY_UP) || LS_UP) && playPauseTimeElapsed >= 0.2f)) && !displayControls) {
                currentPlayPauseState = PLAYPAUSE_RESTART;
                playPauseTimeElapsed = 0.0f;
            }
            break;

        //Display/hide diagnostics section
        case PLAYPAUSE_DIAGNOSTICS:
            //Move up or down a section
            if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_EXIT;
                playPauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_CONTROLS;
                displayControls = false;
                playPauseTimeElapsed = 0.0f;
            }
            break;

        //Exit game section
        case PLAYPAUSE_EXIT:
            //Move up or down a section
            if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_RESUME;
                playPauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_DIAGNOSTICS;
                playPauseTimeElapsed = 0.0f;
            }
            break;
    }

    // Action
    switch (currentPlayPauseState) {
        //Resume game section
        case PLAYPAUSE_RESUME:
            //Resume game
            if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Controller_SetWasPressed_A(true);
                //Unload controls image texture
                UnloadTexture(controlsImg);
                UnloadTexture(keyImg);
                UnloadTexture(mouseImg);
                UnloadTexture(arrowsImg);
                return PLAY_RESUME;
            }
            else if ((HOME_PRESS && !Controller_GetWasPressed_Home()) || ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Controller_GetWasPressed_B())) {
                //Unload controls image texture
                UnloadTexture(controlsImg);
                UnloadTexture(keyImg);
                UnloadTexture(mouseImg);
                UnloadTexture(arrowsImg);
                Controller_SetWasPressed_B(true);
                return PLAY_RESUME;
            }
            UI_ChangeAlpha_Static();
            //Draw menu and update time
            PlayPause_Draw();
            PlayPause_UpdateTime();
            break;

        //Restart game section
        case PLAYPAUSE_RESTART:
            //Restart game
            if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Controller_SetWasPressed_A(true);
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                UnloadTexture(keyImg);
                UnloadTexture(mouseImg);
                UnloadTexture(arrowsImg);
                return PLAY_RESTART;
            }
            //Resume game
            if ((HOME_PRESS && !Controller_GetWasPressed_Home()) || ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Controller_GetWasPressed_B())) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                UnloadTexture(keyImg);
                UnloadTexture(mouseImg);
                UnloadTexture(arrowsImg);
                Controller_SetWasPressed_B(true);
                return PLAY_RESUME;
            }
            UI_ChangeAlpha_Static();
            //Draw menu and update time
            PlayPause_Draw();
            PlayPause_UpdateTime();
            break;

        //View controls section
        case PLAYPAUSE_CONTROLS:
            //Stop or start displaying controls
            if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                displayControls = true;
                Controller_SetWasPressed_A(true);
            }
            //If B is pressed
            else if ((HOME_PRESS && !Controller_GetWasPressed_Home()) || ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Controller_GetWasPressed_B())) {
                //Resume game or stop displaying controls
                if (displayControls == false || HOME_DOWN) {
                    currentPlayPauseState = PLAYPAUSE_RESUME;
                    //Unload controls image texture
                    UnloadTexture(controlsImg);
                    UnloadTexture(keyImg);
                    UnloadTexture(mouseImg);
                    UnloadTexture(arrowsImg);
                    Controller_SetWasPressed_B(true);
                    return PLAY_RESUME;
                }
                else {
                    displayControls = false;
                }
            }
            //Draw controls or pause menu
            if (displayControls == true) {
                PlayPause_DrawControls(Games_GetDisplayed(3));
            }
            else {
                PlayPause_Draw();
            }
            UI_ChangeAlpha_Static();
            //Update time
            PlayPause_UpdateTime();
            break;

        //Display/hide diagnostics section
        case PLAYPAUSE_DIAGNOSTICS:
            //Stop or start displaying diagnostics
            if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Var_SetDiag(!(Var_GetDiag()));
                Var_UpdateUIFile();
                Controller_SetWasPressed_A(true);
            }
            //Resume game
            if ((HOME_PRESS && !Controller_GetWasPressed_Home()) || ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Controller_GetWasPressed_B())) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                UnloadTexture(keyImg);
                UnloadTexture(mouseImg);
                UnloadTexture(arrowsImg);
                Controller_SetWasPressed_B(true);
                return PLAY_RESUME;
            }
            UI_ChangeAlpha_Static();
            //Draw menu and update time
            PlayPause_Draw();
            PlayPause_UpdateTime();
            break;

        //Exit game section
        case PLAYPAUSE_EXIT:
            //Exit game
            if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                UnloadTexture(keyImg);
                UnloadTexture(mouseImg);
                UnloadTexture(arrowsImg);
                Controller_SetWasPressed_A(true);
                return PLAY_EXIT;
            }
            //Resume game
            if ((HOME_PRESS && !Controller_GetWasPressed_Home()) || ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Controller_GetWasPressed_B())) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                UnloadTexture(keyImg);
                UnloadTexture(mouseImg);
                UnloadTexture(arrowsImg);
                Controller_SetWasPressed_B(true);
                return PLAY_RESUME;
            }
            UI_ChangeAlpha_Static();
            //Draw menu and update time
            PlayPause_Draw();
            PlayPause_UpdateTime();
            break;
    }

    return PLAY_PAUSE;
}