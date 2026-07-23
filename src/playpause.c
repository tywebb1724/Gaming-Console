#include "playpause.h"
#include "controlsdisplay_config.h"
#include "playpause_config.h"
#include "controller_config.h"
#include "raylib.h"
#include "config.h"
#include "games.h"

//Current state of the pause screen
static PlayPauseState currentPlayPauseState;
//Monitor width and height
static int monitorWidth;
static int monitorHeight;
//Fonts
static Font fontRegular;
static Font fontBold;
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
static void PlayPause_DrawControls(game_t game) {
    //Draw controller image
    Rectangle sourceRect = {0.0f, 0.0f, (float)controlsImg.width, (float)controlsImg.height};
    Rectangle destRect = {SCREEN_W / 5, SCREEN_H / 5, SCREEN_W / 5 * 3, SCREEN_H / 5 * 3};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(controlsImg, sourceRect, destRect, origin, 0.0f, WHITE);
    //Initialize the text variables for all the controls
    char rDown_Txt[10] = "", rUp_Txt[10] = "", rLeft_Txt[10] = "", rRight_Txt[10] = "";
    char d_Txt[10] = "", rs_Txt[10] = "", back_Txt[10] = "", start_Txt[10] = "";
    char lt_Txt[10] = "", rt_Txt[10] = "", lb_Txt[10] = "", rb_Txt[10] = "", ls_Txt[10] = "";
    char home_Txt[10] = "PAUSE";

    //Draw circles and rectangles for backgrounds of each control
    DrawCircle(RIGHT_FACE_DOWN_X, RIGHT_FACE_DOWN_Y, CONTROL_RADIUS, GRAY);
    DrawCircle(RIGHT_FACE_UP_X, RIGHT_FACE_UP_Y, CONTROL_RADIUS, GRAY);
    DrawCircle(RIGHT_FACE_LEFT_X, RIGHT_FACE_LEFT_Y, CONTROL_RADIUS, GRAY);
    DrawCircle(RIGHT_FACE_RIGHT_X, RIGHT_FACE_RIGHT_Y, CONTROL_RADIUS, GRAY);
    DrawRectangle(HOME_RECT_X, HOME_RECT_Y, HOME_RECT_W, HOME_RECT_H, GRAY);
    DrawRectangle(D_RECT_X, D_RECT_Y, D_RECT_W, D_RECT_H, GRAY);
    DrawRectangle(RS_RECT_X, RS_RECT_Y, RS_RECT_W, RS_RECT_H, GRAY);
    DrawRectangle(BACK_RECT_X, BACK_RECT_Y, BACK_RECT_W, BACK_RECT_H, GRAY);
    DrawRectangle(START_RECT_X, START_RECT_Y, START_RECT_W, START_RECT_H, GRAY);
    DrawRectangle(LT_RECT_X, LT_RECT_Y, LT_RECT_W, LT_RECT_H, GRAY);
    DrawRectangle(RT_RECT_X, RT_RECT_Y, RT_RECT_W, RT_RECT_H, GRAY);
    DrawRectangle(LB_RECT_X, LB_RECT_Y, LB_RECT_W, LB_RECT_H, GRAY);
    DrawRectangle(RB_RECT_X, RB_RECT_Y, RB_RECT_W, RB_RECT_H, GRAY);
    DrawRectangle(LS_RECT_X, LS_RECT_Y, LS_RECT_W, LS_RECT_H, GRAY);

    //Set the text variables for the console and draw the non-shared ones
    if (strcmp(game.console, "Sega Genesis") == 0 || strcmp(game.console, "Sega CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"B");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "MODE");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "C");
        Vector2 rLeft_Size = MeasureTextEx(fontBold, rLeft_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rLeft = {RIGHT_FACE_LEFT_TXT_X, RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(fontBold, rLeft_Txt, rLeft, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "X");
        Vector2 lb_Size = MeasureTextEx(fontBold, lb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lb = {LB_TXT_X, LB_TXT_Y};
        DrawTextEx(fontBold, lb_Txt, lb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "Y");
        Vector2 rUp_Size = MeasureTextEx(fontBold, rUp_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rUp = {RIGHT_FACE_UP_TXT_X, RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(fontBold, rUp_Txt, rUp, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "Z");
        Vector2 rb_Size = MeasureTextEx(fontBold, rb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rb = {RB_TXT_X, RB_TXT_Y};
        DrawTextEx(fontBold, rb_Txt, rb, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Sega Game Gear") == 0 || strcmp(game.console, "Sega Master System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "1");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "2");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
    }
    else if (strcmp(game.console, "TurboGrafx-16") == 0 || strcmp(game.console, "TurboGrafx-CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "RUN");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "I");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "II");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Neo Geo Pocket Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "OPTION");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "B");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
    }
    else if (strcmp(game.console, "Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Game Boy Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Atari Lynx") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Super Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "Y");
        Vector2 rLeft_Size = MeasureTextEx(fontBold, rLeft_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rLeft = {RIGHT_FACE_LEFT_TXT_X, RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(fontBold, rLeft_Txt, rLeft, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "X");
        Vector2 rUp_Size = MeasureTextEx(fontBold, rUp_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rUp = {RIGHT_FACE_UP_TXT_X, RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(fontBold, rUp_Txt, rUp, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(fontBold, lb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lb = {LB_TXT_X, LB_TXT_Y};
        DrawTextEx(fontBold, lb_Txt, lb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(fontBold, rb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rb = {RB_TXT_X, RB_TXT_Y};
        DrawTextEx(fontBold, rb_Txt, rb, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Game Boy Advance") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
        
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(fontBold, lb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lb = {LB_TXT_X, LB_TXT_Y};
        DrawTextEx(fontBold, lb_Txt, lb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(fontBold, rb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rb = {RB_TXT_X, RB_TXT_Y};
        DrawTextEx(fontBold, rb_Txt, rb, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Sony PlayStation") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "X");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "O");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "[]");
        Vector2 rLeft_Size = MeasureTextEx(fontBold, rLeft_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rLeft = {RIGHT_FACE_LEFT_TXT_X, RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(fontBold, rLeft_Txt, rLeft, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "/\\");
        Vector2 rUp_Size = MeasureTextEx(fontBold, rUp_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rUp = {RIGHT_FACE_UP_TXT_X, RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(fontBold, rUp_Txt, rUp, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L1");
        Vector2 lb_Size = MeasureTextEx(fontBold, lb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lb = {LB_TXT_X, LB_TXT_Y};
        DrawTextEx(fontBold, lb_Txt, lb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R1");
        Vector2 rb_Size = MeasureTextEx(fontBold, rb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rb = {RB_TXT_X, RB_TXT_Y};
        DrawTextEx(fontBold, rb_Txt, rb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L2");
        Vector2 lt_Size = MeasureTextEx(fontBold, lt_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lt = {LT_TXT_X, LT_TXT_Y};
        DrawTextEx(fontBold, lt_Txt, lt, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R2");
        Vector2 rt_Size = MeasureTextEx(fontBold, rt_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rt = {RT_TXT_X, RT_TXT_Y};
        DrawTextEx(fontBold, rt_Txt, rt, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Nintendo 64") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(fontBold, lb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lb = {LB_TXT_X, LB_TXT_Y};
        DrawTextEx(fontBold, lb_Txt, lb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(fontBold, rb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rb = {RB_TXT_X, RB_TXT_Y};
        DrawTextEx(fontBold, rb_Txt, rb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "Z");
        Vector2 lt_Size = MeasureTextEx(fontBold, lt_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lt = {LT_TXT_X, LT_TXT_Y};
        DrawTextEx(fontBold, lt_Txt, lt, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rs_Txt,    sizeof(rs_Txt),    "%s", "C");
        Vector2 rs_Size = MeasureTextEx(fontBold, rs_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rs = {RS_TXT_X, RS_TXT_Y};
        DrawTextEx(fontBold, rs_Txt, rs, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "Arcade") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "1");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "2");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "JOY");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "COIN");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "3");
        Vector2 rLeft_Size = MeasureTextEx(fontBold, rLeft_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rLeft = {RIGHT_FACE_LEFT_TXT_X, RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(fontBold, rLeft_Txt, rLeft, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "4");
        Vector2 rUp_Size = MeasureTextEx(fontBold, rUp_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rUp = {RIGHT_FACE_UP_TXT_X, RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(fontBold, rUp_Txt, rUp, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "5");
        Vector2 lb_Size = MeasureTextEx(fontBold, lb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lb = {LB_TXT_X, LB_TXT_Y};
        DrawTextEx(fontBold, lb_Txt, lb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "6");
        Vector2 rb_Size = MeasureTextEx(fontBold, rb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rb = {RB_TXT_X, RB_TXT_Y};
        DrawTextEx(fontBold, rb_Txt, rb, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game.console, "PC") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "ENTER");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "USE");
        snprintf(rDown_Txt,   sizeof(rUp_Txt),   "%s", "RUN");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "MOVE");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "MOVE");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "MAP");
        Vector2 back_Size = MeasureTextEx(fontBold, back_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 back = {BACK_TXT_X, BACK_TXT_Y};
        DrawTextEx(fontBold, back_Txt, back, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rt_Txt, sizeof(rDown_Txt), "%s", "FIRE");
        Vector2 rt_Size = MeasureTextEx(fontBold, rt_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rt = {RT_TXT_X, RT_TXT_Y};
        DrawTextEx(fontBold, rt_Txt, rt, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "WPN-");
        Vector2 lb_Size = MeasureTextEx(fontBold, lb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 lb = {LB_TXT_X, LB_TXT_Y};
        DrawTextEx(fontBold, lb_Txt, lb, CONTROL_SIZE, CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "WPN+");
        Vector2 rb_Size = MeasureTextEx(fontBold, rb_Txt, CONTROL_SIZE, CONTROL_SPACE);
        Vector2 rb = {RB_TXT_X, RB_TXT_Y};
        DrawTextEx(fontBold, rb_Txt, rb, CONTROL_SIZE, CONTROL_SPACE, BLACK);
    }

    //Controls all games use
    Vector2 rDown_Size = MeasureTextEx(fontBold, rDown_Txt, CONTROL_SIZE, CONTROL_SPACE);
    Vector2 rDown = {RIGHT_FACE_DOWN_TXT_X, RIGHT_FACE_DOWN_TXT_Y};
    DrawTextEx(fontBold, rDown_Txt, rDown, CONTROL_SIZE, CONTROL_SPACE, BLACK);

    Vector2 rRight_Size = MeasureTextEx(fontBold, rRight_Txt, CONTROL_SIZE, CONTROL_SPACE);
    Vector2 rRight = {RIGHT_FACE_RIGHT_TXT_X, RIGHT_FACE_RIGHT_TXT_Y};
    DrawTextEx(fontBold, rRight_Txt, rRight, CONTROL_SIZE, CONTROL_SPACE, BLACK);

    Vector2 home_Size = MeasureTextEx(fontBold, home_Txt, CONTROL_SIZE, CONTROL_SPACE);
    Vector2 home = {HOME_TXT_X, HOME_TXT_Y};
    DrawTextEx(fontBold, home_Txt, home, CONTROL_SIZE, CONTROL_SPACE, BLACK);

    Vector2 d_Size = MeasureTextEx(fontBold, d_Txt, CONTROL_SIZE, CONTROL_SPACE);
    Vector2 d = {D_TXT_X, D_TXT_Y};
    DrawTextEx(fontBold, d_Txt, d, CONTROL_SIZE, CONTROL_SPACE, BLACK);

    Vector2 start_Size = MeasureTextEx(fontBold, start_Txt, CONTROL_SIZE, CONTROL_SPACE);
    Vector2 start = {START_TXT_X, START_TXT_Y};
    DrawTextEx(fontBold, start_Txt, start, CONTROL_SIZE, CONTROL_SPACE, BLACK);

    Vector2 ls_Size = MeasureTextEx(fontBold, ls_Txt, CONTROL_SIZE, CONTROL_SPACE);
    Vector2 ls = {LS_TXT_X, LS_TXT_Y};
    DrawTextEx(fontBold, ls_Txt, ls, CONTROL_SIZE, CONTROL_SPACE, BLACK);
}

//Draw the pause screen
static void PlayPause_Draw(bool *displayDiag, Color *themeColor1, Color *themeColor2, Color *themeColor3) {
    //Draw whole section
    Rectangle rectSection = {
        PLAYPAUSE_RECT_X,
        PLAYPAUSE_RECT_Y,
        PLAYPAUSE_RECT_W,
        PLAYPAUSE_RECT_H};
    DrawRectangleRoundedLinesEx(rectSection, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, *themeColor3);
    DrawRectangleRounded(rectSection, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, *themeColor2);
    //Draw title
    Vector2 titleSize = MeasureTextEx(fontBold, PLAYPAUSE_TITLE, PLAYPAUSE_TITLE_SIZE, PLAYPAUSE_SPACE);
    Vector2 title = {PLAYPAUSE_TITLE_X, PLAYPAUSE_TITLE_Y};
    DrawTextEx(fontBold, PLAYPAUSE_TITLE, title, PLAYPAUSE_TITLE_SIZE, PLAYPAUSE_SPACE, *themeColor3);
    //Draw resume
    Vector2 resumeSize = MeasureTextEx(fontRegular, RESUME_TXT, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE);
    Vector2 resume = {PLAYPAUSE_X, RESUME_Y};
    Rectangle rectResume = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_RESUME_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectResume, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, *themeColor3);
    DrawTextEx(fontRegular, RESUME_TXT, resume, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, *themeColor3);
    //Draw restart
    Vector2 restart = {PLAYPAUSE_X, RESTART_Y};
    Rectangle rectRestart = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_RESTART_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectRestart, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, *themeColor3);
    DrawTextEx(fontRegular, RESTART_TXT, restart, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, *themeColor3);
    //Draw controls
    Vector2 controls = {PLAYPAUSE_X, CONTROLS_Y};
    Rectangle rectControls = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_CONTROLS_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectControls, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, *themeColor3);
    DrawTextEx(fontRegular, CONTROLS_TXT, controls, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, *themeColor3);
    //Draw display/hide diagnostics
    Vector2 diagnostics = {PLAYPAUSE_X, DIAGNOSTICS_Y};
    Rectangle rectDiag = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_DIAGNOSTICS_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDiag, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, *themeColor3);
    char *diagTxt;
    if (*displayDiag)
    {
        diagTxt = HIDE_DIAGNOSTICS_TXT;
    }
    else
    {
        diagTxt = DISPLAY_DIAGNOSTICS_TXT;
    }
    DrawTextEx(fontRegular, diagTxt, diagnostics, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, *themeColor3);
    //Draw exit
    Vector2 exitSize = MeasureTextEx(fontRegular, EXIT_TXT, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE);
    Vector2 exit = {PLAYPAUSE_X, EXIT_Y};
    Rectangle rectExit = {
        PLAYPAUSE_SELECT_RECT_X,
        PLAYPAUSE_EXIT_RECT_Y,
        PLAYPAUSE_SELECT_RECT_W,
        PLAYPAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectExit, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, PLAYPAUSE_LINE_THICK, *themeColor3);
    DrawTextEx(fontRegular, EXIT_TXT, exit, PLAYPAUSE_SIZE, PLAYPAUSE_SPACE, *themeColor3);
   
    //Draw select
    Vector2 selectSize = MeasureTextEx(fontRegular, SELECT_TXT, SELECT_SIZE, PLAYPAUSE_SPACE);
    Vector2 select = {SELECT_X, SELECT_Y};
    DrawTextEx(fontRegular, SELECT_TXT, select, SELECT_SIZE, PLAYPAUSE_SPACE, *themeColor3);
    //Draw back
    Vector2 backSize = MeasureTextEx(fontRegular, BACK_TXT, BACK_SIZE, PLAYPAUSE_SPACE);
    Vector2 back = {BACK_X, BACK_Y};
    DrawTextEx(fontRegular, BACK_TXT, back, BACK_SIZE, PLAYPAUSE_SPACE, *themeColor3);
    //Draw an outline around the selected section
    if (currentPlayPauseState == PLAYPAUSE_RESUME)
    {
        DrawRectangleRoundedLinesEx(rectResume, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, *themeColor1);
    }
    else if (currentPlayPauseState == PLAYPAUSE_RESTART)
    {
        DrawRectangleRoundedLinesEx(rectRestart, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, *themeColor1);
    }
    else if (currentPlayPauseState == PLAYPAUSE_CONTROLS)
    {
        DrawRectangleRoundedLinesEx(rectControls, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, *themeColor1);
    }
    else if (currentPlayPauseState == PLAYPAUSE_DIAGNOSTICS)
    {
        DrawRectangleRoundedLinesEx(rectDiag, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, *themeColor1);
    }
    else
    {
        DrawRectangleRoundedLinesEx(rectExit, PLAYPAUSE_ROUND, PLAYPAUSE_SEGMENTS, 2 * PLAYPAUSE_LINE_THICK, *themeColor1);
    }
}

//PlayPause initialization
void PlayPause_Init() {
    //Get width and height of the monitor
    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);
    //Load the fonts into this file
    fontRegular = LoadFont("assets/fonts/Exo2-Regular.ttf");
    fontBold = LoadFont("assets/fonts/Exo2-Bold.ttf");
    //Load controller image
    controlsImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/controller.png");
    //Time elapsed on selected section
    playPauseTimeElapsed = 0.0f;
    //Start pause menu at top section
    currentPlayPauseState = PLAYPAUSE_RESUME;
}

//PlayPause tick
PlayState PlayPause_Tick(bool *displayDiag, Color *themeColor1, Color *themeColor2, Color *themeColor3) {
    //static bool wasHomePressed;

    //if (!HOME_HOLD) {
        //wasHomePressed = false;
    //}


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
            if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && playPauseTimeElapsed >= 0.2f)) {
                currentPlayPauseState = PLAYPAUSE_DIAGNOSTICS;
                playPauseTimeElapsed = 0.0f;
            }
            else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && playPauseTimeElapsed >= 0.2f)) {
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
            if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_DOWN) {
                // mouseWasPressed = false;
                return PLAY_RESUME;
            }
            //Resume game
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                /// mouseWasPressed = true;
                return PLAY_RESUME;
            }
            //Draw menu and update time
            PlayPause_Draw(displayDiag, themeColor1, themeColor2, themeColor3);
            PlayPause_UpdateTime();
            break;

        //Restart game section
        case PLAYPAUSE_RESTART:
            if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_DOWN) {
                // mouseWasPressed = false;
            }
            //Restart game
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                return PLAY_RESTART;
            }
            //Resume game
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                return PLAY_RESUME;
            }
            //Draw menu and update time
            PlayPause_Draw(displayDiag, themeColor1, themeColor2, themeColor3);
            PlayPause_UpdateTime();
            break;

        //View controls section
        case PLAYPAUSE_CONTROLS:
            if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_DOWN) {
                // mouseWasPressed = false;
            }
            //Stop or start displaying controls
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)) {
                displayControls = true;
                //mouseLeftWasPressed = true;
            }
            //If B is pressed
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                //Resume game or stop displaying controls
                if (displayControls == false) {
                    /// mouseWasPressed = true;
                    currentPlayPauseState = PLAYPAUSE_RESUME;
                    return PLAY_RESUME;
                }
                else {
                    displayControls = false;
                }
            }
            //Draw controls or pause menu
            if (displayControls == true) {
                PlayPause_DrawControls(*Games_GetDisplayed(3));
            }
            else {
                PlayPause_Draw(displayDiag, themeColor1, themeColor2, themeColor3);
            }
            //Update time
            PlayPause_UpdateTime();
            break;

        //Display/hide diagnostics section
        case PLAYPAUSE_DIAGNOSTICS:
            if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_DOWN) {
                // mouseWasPressed = false;
            }
            //Stop or start displaying diagnostics
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) {
                *displayDiag = !(*displayDiag);
                ///////////Update_UI_Txt();
            }
            //Resume game
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                return PLAY_RESUME;
            }
            //Draw menu and update time
            PlayPause_Draw(displayDiag, themeColor1, themeColor2, themeColor3);
            PlayPause_UpdateTime();
            break;

        //Exit game section
        case PLAYPAUSE_EXIT:
            if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_DOWN) {
                // mouseWasPressed = false;
            }
            //Exit game
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) {
                currentPlayPauseState = PLAYPAUSE_RESUME;
                return PLAY_EXIT;
            }
            //Resume game
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) {
                /// mouseWasPressed = true;
                currentPlayPauseState = PLAYPAUSE_RESUME;
                return PLAY_RESUME;
            }
            //Draw menu and update time
            PlayPause_Draw(displayDiag, themeColor1, themeColor2, themeColor3);
            PlayPause_UpdateTime();
            break;
    }

    return GAME_PAUSE;
}