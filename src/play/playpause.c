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

//Current state of the pause screen
static PlayPauseState currentPlayPauseState;
//Controller image
static Texture2D controlsImg;
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
    Rectangle sourceRect = {0.0f, 0.0f, (float)controlsImg.width, (float)controlsImg.height};
    Rectangle destRect = {Var_GetMonitorWidth() / 5, Var_GetMonitorHeight() / 5, Var_GetMonitorWidth() / 5 * 3, Var_GetMonitorHeight() / 5 * 3};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(controlsImg, sourceRect, destRect, origin, 0.0f, WHITE);
    //Initialize the text variables for all the controls
    char rDown_Txt[10] = "", rUp_Txt[10] = "", rLeft_Txt[10] = "", rRight_Txt[10] = "";
    char d_Txt[10] = "", rs_Txt[10] = "", back_Txt[10] = "", start_Txt[10] = "";
    char lt_Txt[10] = "", rt_Txt[10] = "", lb_Txt[10] = "", rb_Txt[10] = "", ls_Txt[10] = "";
    char home_Txt[10] = "PAUSE";

    //Draw circles and rectangles for backgrounds of each control
    DrawCircle(PLAYPAUSE_RIGHT_FACE_DOWN_X, PLAYPAUSE_RIGHT_FACE_DOWN_Y, PLAYPAUSE_CONTROL_RADIUS, GRAY);
    DrawCircle(PLAYPAUSE_RIGHT_FACE_UP_X, PLAYPAUSE_RIGHT_FACE_UP_Y, PLAYPAUSE_CONTROL_RADIUS, GRAY);
    DrawCircle(PLAYPAUSE_RIGHT_FACE_LEFT_X, PLAYPAUSE_RIGHT_FACE_LEFT_Y, PLAYPAUSE_CONTROL_RADIUS, GRAY);
    DrawCircle(PLAYPAUSE_RIGHT_FACE_RIGHT_X, PLAYPAUSE_RIGHT_FACE_RIGHT_Y, PLAYPAUSE_CONTROL_RADIUS, GRAY);
    DrawRectangle(PLAYPAUSE_HOME_RECT_X, PLAYPAUSE_HOME_RECT_Y, PLAYPAUSE_HOME_RECT_W, PLAYPAUSE_HOME_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_D_RECT_X, PLAYPAUSE_D_RECT_Y, PLAYPAUSE_D_RECT_W, PLAYPAUSE_D_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_RS_RECT_X, PLAYPAUSE_RS_RECT_Y, PLAYPAUSE_RS_RECT_W, PLAYPAUSE_RS_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_BACK_RECT_X, PLAYPAUSE_BACK_RECT_Y, PLAYPAUSE_BACK_RECT_W, PLAYPAUSE_BACK_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_START_RECT_X, PLAYPAUSE_START_RECT_Y, PLAYPAUSE_START_RECT_W, PLAYPAUSE_START_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_LT_RECT_X, PLAYPAUSE_LT_RECT_Y, PLAYPAUSE_LT_RECT_W, PLAYPAUSE_LT_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_RT_RECT_X, PLAYPAUSE_RT_RECT_Y, PLAYPAUSE_RT_RECT_W, PLAYPAUSE_RT_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_LB_RECT_X, PLAYPAUSE_LB_RECT_Y, PLAYPAUSE_LB_RECT_W, PLAYPAUSE_LB_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_RB_RECT_X, PLAYPAUSE_RB_RECT_Y, PLAYPAUSE_RB_RECT_W, PLAYPAUSE_RB_RECT_H, GRAY);
    DrawRectangle(PLAYPAUSE_LS_RECT_X, PLAYPAUSE_LS_RECT_Y, PLAYPAUSE_LS_RECT_W, PLAYPAUSE_LS_RECT_H, GRAY);

    //Set the text variables for the console and draw the non-shared ones
    if (strcmp((*game).console, "Sega Genesis") == 0 || strcmp((*game).console, "Sega CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"B");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "MODE");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "C");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rLeft = {PLAYPAUSE_RIGHT_FACE_LEFT_TXT_X, PLAYPAUSE_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "X");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "Y");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rUp = {PLAYPAUSE_RIGHT_FACE_UP_TXT_X, PLAYPAUSE_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "Z");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Sega Game Gear") == 0 || strcmp((*game).console, "Sega Master System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "1");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "2");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
    }
    else if (strcmp((*game).console, "TurboGrafx-16") == 0 || strcmp((*game).console, "TurboGrafx-CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "RUN");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "I");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "II");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Neo Geo Pocket Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "OPTION");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "B");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
    }
    else if (strcmp((*game).console, "Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Game Boy Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Atari Lynx") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Super Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "Y");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rLeft = {PLAYPAUSE_RIGHT_FACE_LEFT_TXT_X, PLAYPAUSE_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "X");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rUp = {PLAYPAUSE_RIGHT_FACE_UP_TXT_X, PLAYPAUSE_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Game Boy Advance") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
        
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Sony PlayStation") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "X");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "O");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "[]");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rLeft = {PLAYPAUSE_RIGHT_FACE_LEFT_TXT_X, PLAYPAUSE_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "/\\");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rUp = {PLAYPAUSE_RIGHT_FACE_UP_TXT_X, PLAYPAUSE_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L1");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R1");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L2");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lt = {PLAYPAUSE_LT_TXT_X, PLAYPAUSE_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R2");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rt = {PLAYPAUSE_RT_TXT_X, PLAYPAUSE_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Nintendo 64") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "Z");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lt = {PLAYPAUSE_LT_TXT_X, PLAYPAUSE_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rs_Txt,    sizeof(rs_Txt),    "%s", "C");
        Vector2 rs_Size = MeasureTextEx(Var_GetFontBold(), rs_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rs = {PLAYPAUSE_RS_TXT_X, PLAYPAUSE_RS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rs_Txt, rs, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "Arcade") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "1");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "2");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "JOY");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "COIN");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "3");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rLeft = {PLAYPAUSE_RIGHT_FACE_LEFT_TXT_X, PLAYPAUSE_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "4");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rUp = {PLAYPAUSE_RIGHT_FACE_UP_TXT_X, PLAYPAUSE_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "5");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "6");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
    }
    else if (strcmp((*game).console, "PC") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "ENTER");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "USE");
        snprintf(rDown_Txt,   sizeof(rDown_Txt),   "%s", "RUN");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "MOVE");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "MOVE");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "MAP");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 back = {PLAYPAUSE_BACK_TXT_X, PLAYPAUSE_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rt_Txt, sizeof(rt_Txt), "%s", "FIRE");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rt = {PLAYPAUSE_RT_TXT_X, PLAYPAUSE_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "WPN-");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 lb = {PLAYPAUSE_LB_TXT_X, PLAYPAUSE_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "WPN+");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
        Vector2 rb = {PLAYPAUSE_RB_TXT_X, PLAYPAUSE_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
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

    Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 d = {PLAYPAUSE_D_TXT_X, PLAYPAUSE_D_TXT_Y};
    DrawTextEx(Var_GetFontBold(), d_Txt, d, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

    Vector2 start_Size = MeasureTextEx(Var_GetFontBold(), start_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 start = {PLAYPAUSE_START_TXT_X, PLAYPAUSE_START_TXT_Y};
    DrawTextEx(Var_GetFontBold(), start_Txt, start, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);

    Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE);
    Vector2 ls = {PLAYPAUSE_LS_TXT_X, PLAYPAUSE_LS_TXT_Y};
    DrawTextEx(Var_GetFontBold(), ls_Txt, ls, PLAYPAUSE_CONTROL_SIZE, PLAYPAUSE_CONTROL_SPACE, BLACK);
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
    if (Var_GetDiag())
    {
        diagTxt = HIDE_DIAGNOSTICS_TXT;
    }
    else
    {
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
    DrawTextEx(Var_GetFontRegular(), SELECT_TXT, select, SELECT_SIZE, PLAYPAUSE_SPACE, Var_GetColor3());
    //Draw back
    Vector2 backSize = MeasureTextEx(Var_GetFontRegular(), BACK_TXT, BACK_SIZE, PLAYPAUSE_SPACE);
    Vector2 back = {BACK_X, BACK_Y};
    DrawTextEx(Var_GetFontRegular(), BACK_TXT, back, BACK_SIZE, PLAYPAUSE_SPACE, Var_GetColor3());
    //Draw an outline around the selected section
    if (currentPlayPauseState == PLAYPAUSE_RESUME)
    {
        DrawRectangleRoundedLinesEx(rectResume, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
    else if (currentPlayPauseState == PLAYPAUSE_RESTART)
    {
        DrawRectangleRoundedLinesEx(rectRestart, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
    else if (currentPlayPauseState == PLAYPAUSE_CONTROLS)
    {
        DrawRectangleRoundedLinesEx(rectControls, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
    else if (currentPlayPauseState == PLAYPAUSE_DIAGNOSTICS)
    {
        DrawRectangleRoundedLinesEx(rectDiag, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
    else
    {
        DrawRectangleRoundedLinesEx(rectExit, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, Var_GetColor1());
    }
}

//PlayPause initialization
void PlayPause_Init() {
    //Load controller image
    controlsImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/controller.png");
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
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Controller_SetWasPressed_A(true);
                //Unload controls image texture
                UnloadTexture(controlsImg);
                return PLAY_RESUME;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                //Unload controls image texture
                UnloadTexture(controlsImg);
                return PLAY_RESUME;
            }
            //Draw menu and update time
            PlayPause_Draw();
            PlayPause_UpdateTime();
            break;

        //Restart game section
        case PLAYPAUSE_RESTART:
            //Restart game
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Controller_SetWasPressed_A(true);
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                return PLAY_RESTART;
            }
            //Resume game
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                return PLAY_RESUME;
            }
            //Draw menu and update time
            PlayPause_Draw();
            PlayPause_UpdateTime();
            break;

        //View controls section
        case PLAYPAUSE_CONTROLS:
            //Stop or start displaying controls
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
                displayControls = true;
                Controller_SetWasPressed_A(true);
            }
            //If B is pressed
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                //Resume game or stop displaying controls
                if (displayControls == false) {
                    currentPlayPauseState = PLAYPAUSE_RESUME;
                    //Unload controls image texture
                    UnloadTexture(controlsImg);
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
            //Update time
            PlayPause_UpdateTime();
            break;

        //Display/hide diagnostics section
        case PLAYPAUSE_DIAGNOSTICS:
            //Stop or start displaying diagnostics
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
                Var_SetDiag(!(Var_GetDiag()));
                Var_UpdateUIFile();
                Controller_SetWasPressed_A(true);
            }
            //Resume game
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                return PLAY_RESUME;
            }
            //Draw menu and update time
            PlayPause_Draw();
            PlayPause_UpdateTime();
            break;

        //Exit game section
        case PLAYPAUSE_EXIT:
            //Exit game
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                Controller_SetWasPressed_A(true);
                return PLAY_EXIT;
            }
            //Resume game
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                //Unload controls image texture
                UnloadTexture(controlsImg);
                return PLAY_RESUME;
            }
            //Draw menu and update time
            PlayPause_Draw();
            PlayPause_UpdateTime();
            break;
    }

    return PLAY_PAUSE;
}