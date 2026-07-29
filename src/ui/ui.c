#include "ui.h"
#include "raylib.h"
#include "raymath.h"
#include <string.h>
#include <math.h>
#include "states.h"
#include "categories.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "var.h"
#include "controller_config.h"
#include "uipause.h"
#include "brightness.h"
#include "uipause_config.h"
#include "ui_config.h"
#include "games.h"
#include "config.h"
#include "play/playpause.h"

//Length of longest text line in the select box
static float maxLen;
//Current state of the UI
static UIState currentUIState;
//Spider logo
static Texture2D spiderLogo;
//Variable to hold the current image to draw
Texture2D img;
//Variables to hold information about the current image to draw
static float img_X, img_Y, img_W, img_H;
//Different alphas
static float alphaGames;
static float alphaCategories_Out;
static float alphaCategories_In;
static float alphaSelectBox;
static float alphaSelectTxt_TimeElapsed;
//Whether the text is blinking on or not
static bool alphaSelectTxt_Blink = true;
//Image of controller
static Texture2D controlsImg;


//Set the max length variable
void UI_SetMaxLen(float value) {
    maxLen = value;
}

//Center an image on a certain X position
static float UI_CenterImg_X(float width, float position) {
    return (position - width / 2.0f);
}

//Center an image on a certain Y position
static float UI_CenterImg_Y(float height, float position) {
    return (position - height / 2.0f);
}

//Reset the coordinates for the displayed games after games scroll
static void UI_ResetCoords_Games() {
    // Reset x coordinates
    Games_GetDisplayed(0)->x = LEFT3_GAME_X;
    Games_GetDisplayed(1)->x = LEFT2_GAME_X;
    Games_GetDisplayed(2)->x = LEFT1_GAME_X;
    Games_GetDisplayed(3)->x = CENTER_GAME_X;
    Games_GetDisplayed(4)->x = RIGHT1_GAME_X;
    Games_GetDisplayed(5)->x = RIGHT2_GAME_X;
    Games_GetDisplayed(6)->x = RIGHT3_GAME_X;
    // Reset y coordinates
    Games_GetDisplayed(0)->y = SIDE3_GAME_Y;
    Games_GetDisplayed(1)->y = SIDE2_GAME_Y;
    Games_GetDisplayed(2)->y = SIDE1_GAME_Y;
    Games_GetDisplayed(3)->y = CENTER_GAME_Y;
    Games_GetDisplayed(4)->y = SIDE1_GAME_Y;
    Games_GetDisplayed(5)->y = SIDE2_GAME_Y;
    Games_GetDisplayed(6)->y = SIDE3_GAME_Y;
    // Reset width values
    Games_GetDisplayed(0)->w = SIDE3_GAME_W;
    Games_GetDisplayed(1)->w = SIDE2_GAME_W;
    Games_GetDisplayed(2)->w = SIDE1_GAME_W;
    Games_GetDisplayed(3)->w = CENTER_GAME_W;
    Games_GetDisplayed(4)->w = SIDE1_GAME_W;
    Games_GetDisplayed(5)->w = SIDE2_GAME_W;
    Games_GetDisplayed(6)->w = SIDE3_GAME_W;
    // Reset height values
    Games_GetDisplayed(0)->h = SIDE3_GAME_H;
    Games_GetDisplayed(1)->h = SIDE2_GAME_H;
    Games_GetDisplayed(2)->h = SIDE1_GAME_H;
    Games_GetDisplayed(3)->h = CENTER_GAME_H;
    Games_GetDisplayed(4)->h = SIDE1_GAME_H;
    Games_GetDisplayed(5)->h = SIDE2_GAME_H;
    Games_GetDisplayed(6)->h = SIDE3_GAME_H;
}

//Reset the coordinates for the displayed games after category scroll
static void UI_ResetCoords_Categ() {
    //If scrolling to the right
    if (Var_GetScrollCateg() == SCROLL_RIGHT) {
        //Reset x coordinates
        Games_GetNew(0)->x = LEFT2_SCROLLL_X;
        Games_GetNew(1)->x = LEFT1_SCROLLL_X;
        Games_GetNew(2)->x = CENTER_SCROLLL_X;
        Games_GetNew(3)->x = RIGHT1_SCROLLL_X;
        Games_GetNew(4)->x = RIGHT2_SCROLLL_X;
    }
    //If scrolling to the left
    else {
        //Reset x coordinates
        Games_GetNew(0)->x = LEFT2_SCROLLR_X;
        Games_GetNew(1)->x = LEFT1_SCROLLR_X;
        Games_GetNew(2)->x = CENTER_SCROLLR_X;
        Games_GetNew(3)->x = RIGHT1_SCROLLR_X;
        Games_GetNew(4)->x = RIGHT2_SCROLLR_X;
    }
    //Reset y coordinates
    Games_GetNew(0)->y = SIDE2_GAME_Y;
    Games_GetNew(1)->y = SIDE1_GAME_Y;
    Games_GetNew(2)->y = CENTER_GAME_Y;
    Games_GetNew(3)->y = SIDE1_GAME_Y;
    Games_GetNew(4)->y = SIDE2_GAME_Y;
    //Reset width values
    Games_GetNew(0)->w = SIDE2_GAME_W;
    Games_GetNew(1)->w = SIDE1_GAME_W;
    Games_GetNew(2)->w = CENTER_GAME_W;
    Games_GetNew(3)->w = SIDE1_GAME_W;
    Games_GetNew(4)->w = SIDE2_GAME_W;
    //Reset height values
    Games_GetNew(0)->h = SIDE2_GAME_H;
    Games_GetNew(1)->h = SIDE1_GAME_H;
    Games_GetNew(2)->h = CENTER_GAME_H;
    Games_GetNew(3)->h = SIDE1_GAME_H;
    Games_GetNew(4)->h = SIDE2_GAME_H;
}

//Draw an image
static void UI_DrawImage(float alpha) {
    //If image loaded, draw image
    if (img.id > 0) {
        Rectangle sourceRect = {0.0f, 0.0f, (float)img.width, (float)img.height};
        Rectangle destRect = {(float)img_X, img_Y, img_W, img_H};
        Vector2 origin = {0.0f, 0.0f};
        DrawTexturePro(img, sourceRect, destRect, origin, 0.0f, Fade(WHITE, alpha));
    }
    //Draw gray placeholder if image isn't loaded
    else {
        DrawRectangle(img_X, img_Y, img_W, img_H, Fade(LIGHTGRAY, alpha));
        DrawRectangleLines(img_X, img_Y, img_W, img_H, Fade(GRAY, alpha));
    }
}

//Draw options button at top right of screen
void UI_DrawOptions() {
    // Draw options button
    DrawCircle(OPTIONS_BTN_X, OPTIONS_BTN_Y, OPTIONS_RADIUS, Var_GetColor3());
    Vector2 start1 = {OPTIONS_LINE_START_X, OPTIONS_LINE1_Y};
    Vector2 end1 = {OPTIONS_LINE_END_X, OPTIONS_LINE1_Y};
    DrawLineEx(start1, end1, OPTIONS_THICKNESS_LINE2, Var_GetColor2());
    Vector2 start2 = {OPTIONS_LINE_START_X, OPTIONS_LINE2_Y};
    Vector2 end2 = {OPTIONS_LINE_END_X, OPTIONS_LINE2_Y};
    DrawLineEx(start2, end2, OPTIONS_THICKNESS_LINE1, Var_GetColor2());
    Vector2 start3 = {OPTIONS_LINE_START_X, OPTIONS_LINE3_Y};
    Vector2 end3 = {OPTIONS_LINE_END_X, OPTIONS_LINE3_Y};
    DrawLineEx(start3, end3, OPTIONS_THICKNESS_LINE3, Var_GetColor2());
    //Draw options text
    Vector2 optionsSize = MeasureTextEx(Var_GetFontBold(), OPTIONS_TXT, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE);
    Vector2 options = {OPTIONS_TXT_X, OPTIONS_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), OPTIONS_TXT, options, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE, Var_GetColor3());
}

//Function to draw the bumpers
static void UI_DrawBumpers() {
    //Values for left bumper
    Rectangle bumperLRec = {
        BUMPER_L_X,
        BUMPER_Y,
        BUMPER_W,
        BUMPER_H};
    //Values for right bumper
    Rectangle bumperRRec = {
        BUMPER_R_X,
        BUMPER_Y,
        BUMPER_W,
        BUMPER_H};

    //Values for bumper text and drawing the text
    Vector2 bumperLSize = MeasureTextEx(Var_GetFontBold(), BUMPER_L_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperRSize = MeasureTextEx(Var_GetFontBold(), BUMPER_R_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperR = {BUMPER_R_TXT_X, BUMPER_TXT_Y};
    Vector2 bumperL = {BUMPER_L_TXT_X, BUMPER_TXT_Y};
    //Draw bumpers
    if (Var_GetScrollCateg() == SCROLL_RIGHT) {
        DrawRectangleRounded(bumperLRec, BUMPER_ROUND, BUMPER_SEGMENTS, Fade(Var_GetColor2(), alphaCategories_Out));
        DrawRectangleRounded(bumperRRec, BUMPER_ROUND, BUMPER_SEGMENTS, Var_GetColor3());
        DrawTextEx(Var_GetFontBold(), BUMPER_R_TXT, bumperR, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Var_GetColor2());
        DrawTextEx(Var_GetFontBold(), BUMPER_L_TXT, bumperL, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Fade(Var_GetColor2(), alphaCategories_Out));
    }
    else if (Var_GetScrollCateg() == SCROLL_LEFT) {
        DrawRectangleRounded(bumperLRec, BUMPER_ROUND, BUMPER_SEGMENTS, Var_GetColor3());
        DrawRectangleRounded(bumperRRec, BUMPER_ROUND, BUMPER_SEGMENTS, Fade(Var_GetColor2(), alphaCategories_Out));
        DrawTextEx(Var_GetFontBold(), BUMPER_R_TXT, bumperR, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Fade(Var_GetColor2(), alphaCategories_Out));
        DrawTextEx(Var_GetFontBold(), BUMPER_L_TXT, bumperL, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Var_GetColor2());
    }
    else {
        DrawRectangleRounded(bumperLRec, BUMPER_ROUND, BUMPER_SEGMENTS, Var_GetColor3());
        DrawRectangleRounded(bumperRRec, BUMPER_ROUND, BUMPER_SEGMENTS, Var_GetColor3());
        DrawTextEx(Var_GetFontBold(), BUMPER_R_TXT, bumperR, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Var_GetColor2());
        DrawTextEx(Var_GetFontBold(), BUMPER_L_TXT, bumperL, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Var_GetColor2());
    }
}

//Draw bumpers, option button, and small spider logo
static void UI_DrawTop() {
    UI_DrawBumpers();
    UI_DrawOptions();
    //Draw small spider logo
    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(SMALL_LOGO_SIZE, CENTER_X), UI_CenterImg_Y(SMALL_LOGO_SIZE, SMALL_LOGO_Y), SMALL_LOGO_SIZE, SMALL_LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Draw the select controls at the bottom
static void UI_DrawBottom() {
    //Draw left arrow (LS)
    DrawCircle(LS_LEFT_X + LS_OFFSET, LS_Y, LS_RADIUS, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    Vector2 point1 = {LS_LEFT_X - (LS_TRIANGLE_SIZE / 2), LS_Y};
    Vector2 point2 = {LS_LEFT_X + (LS_TRIANGLE_SIZE / 2), LS_Y + LS_TRIANGLE_SIZE / 2};
    Vector2 point3 = {LS_LEFT_X + (LS_TRIANGLE_SIZE / 2), LS_Y - LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point1, point2, point3, Fade(Var_GetColor2(), Var_GetAlphaSelect()));
    //Draw right arrow (RS)
    DrawCircle(LS_RIGHT_X - LS_OFFSET, LS_Y, LS_RADIUS, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    Vector2 point4 = {LS_RIGHT_X + (LS_TRIANGLE_SIZE / 2), LS_Y};
    Vector2 point5 = {LS_RIGHT_X - (LS_TRIANGLE_SIZE / 2), LS_Y + LS_TRIANGLE_SIZE / 2};
    Vector2 point6 = {LS_RIGHT_X - (LS_TRIANGLE_SIZE / 2), LS_Y - LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point6, point5, point4, Fade(Var_GetColor2(), Var_GetAlphaSelect()));
    //Values for Text
    Vector2 gameSize = MeasureTextEx(Var_GetFontBold(), Games_GetDisplayed(3)->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 bottomSize = MeasureTextEx(Var_GetFontBold(), BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 game = {BOTTOM_TXT_X, GAME_TXT_Y};
    Vector2 bottom = {BOTTOM_TXT_X, BOTTOM_TXT_Y};
    Vector2 console = {BOTTOM_TXT_X, CONSOLE_TXT_Y};
    //Draw section
    Rectangle rect = {
        BOTTOM_RECT_X,
        BOTTOM_RECT_Y,
        BOTTOM_RECT_W,
        BOTTOM_RECT_H};
    DrawRectangleRounded(rect, BOTTOM_ROUND, BOTTOM_SEGMENTS, Fade(Var_GetColor2(), alphaSelectBox));
    DrawRectangleRoundedLinesEx(rect, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_BOTTOM, Fade(Var_GetColor3(), alphaSelectBox));
    //Draw text
    DrawTextEx(Var_GetFontBold(), Games_GetDisplayed(3)->title, game, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    DrawTextEx(Var_GetFontBold(), Games_GetDisplayed(3)->console, console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    DrawTextEx(Var_GetFontBold(), BOTTOM_TXT, bottom, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
}

//Load spider logo
void UI_LoadLogo() {
    //Load the logo textures
    spiderLogo = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/LogoBlack.png");
}

//Draw the boot screen
void UI_DrawBootScreen() {
    //Clear background
    ClearBackground(BACKGROUND_CLR);
    //Draw logo
    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(LOGO_SIZE, CENTER_X), UI_CenterImg_Y(LOGO_SIZE, Var_GetMonitorHeight() / 2), LOGO_SIZE, LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Load image of controller
void UI_LoadControlImg() {
    controlsImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/controller.png");
}

//Draw controls for libretro game
static void UI_DrawControls_Lib(const game_t* game) {
    //Draw controller image
    Rectangle sourceRect = {0.0f, 0.0f, (float)controlsImg.width, (float)controlsImg.height};
    Rectangle destRect = {Var_GetMonitorWidth() / 5, Var_GetMonitorHeight() / 3, Var_GetMonitorWidth() / 5 * 3, Var_GetMonitorHeight() / 5 * 3};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(controlsImg, sourceRect, destRect, origin, 0.0f, WHITE);
    //Initialize the text variables for all the controls
    char rDown_Txt[10] = "", rUp_Txt[10] = "", rLeft_Txt[10] = "", rRight_Txt[10] = "";
    char d_Txt[10] = "", rs_Txt[10] = "", back_Txt[10] = "", start_Txt[10] = "";
    char lt_Txt[10] = "", rt_Txt[10] = "", lb_Txt[10] = "", rb_Txt[10] = "", ls_Txt[10] = "";
    char home_Txt[10] = "PAUSE";

    //Draw circles and rectangles for backgrounds of each control
    DrawCircle(LAUNCH_RIGHT_FACE_DOWN_X, LAUNCH_RIGHT_FACE_DOWN_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_UP_X, LAUNCH_RIGHT_FACE_UP_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_LEFT_X, LAUNCH_RIGHT_FACE_LEFT_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_RIGHT_X, LAUNCH_RIGHT_FACE_RIGHT_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawRectangle(LAUNCH_HOME_RECT_X, LAUNCH_HOME_RECT_Y, LAUNCH_HOME_RECT_W, LAUNCH_HOME_RECT_H, GRAY);
    DrawRectangle(LAUNCH_D_RECT_X, LAUNCH_D_RECT_Y, LAUNCH_D_RECT_W, LAUNCH_D_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RS_RECT_X, LAUNCH_RS_RECT_Y, LAUNCH_RS_RECT_W, LAUNCH_RS_RECT_H, GRAY);
    DrawRectangle(LAUNCH_BACK_RECT_X, LAUNCH_BACK_RECT_Y, LAUNCH_BACK_RECT_W, LAUNCH_BACK_RECT_H, GRAY);
    DrawRectangle(LAUNCH_START_RECT_X, LAUNCH_START_RECT_Y, LAUNCH_START_RECT_W, LAUNCH_START_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LT_RECT_X, LAUNCH_LT_RECT_Y, LAUNCH_LT_RECT_W, LAUNCH_LT_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RT_RECT_X, LAUNCH_RT_RECT_Y, LAUNCH_RT_RECT_W, LAUNCH_RT_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LB_RECT_X, LAUNCH_LB_RECT_Y, LAUNCH_LB_RECT_W, LAUNCH_LB_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RB_RECT_X, LAUNCH_RB_RECT_Y, LAUNCH_RB_RECT_W, LAUNCH_RB_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LS_RECT_X, LAUNCH_LS_RECT_Y, LAUNCH_LS_RECT_W, LAUNCH_LS_RECT_H, GRAY);

    //Set the text variables for the console and draw the non-shared ones
    if (strcmp(game->console, "Sega Genesis") == 0 || strcmp(game->console, "Sega CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"B");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "MODE");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "C");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "X");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "Y");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "Z");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sega Game Gear") == 0 || strcmp(game->console, "Sega Master System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "1");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "2");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
    }
    else if (strcmp(game->console, "TurboGrafx-16") == 0 || strcmp(game->console, "TurboGrafx-CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "RUN");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "I");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "II");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Neo Geo Pocket Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "OPTION");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "B");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
    }
    else if (strcmp(game->console, "Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Game Boy Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Atari Lynx") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Super Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "Y");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "X");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Game Boy Advance") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");
        
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sony PlayStation") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "X");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "O");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "[]");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "/\\");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L1");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R1");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L2");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lt = {LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R2");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rt = {LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Nintendo 64") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "Z");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lt = {LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rs_Txt,    sizeof(rs_Txt),    "%s", "C");
        Vector2 rs_Size = MeasureTextEx(Var_GetFontBold(), rs_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rs = {LAUNCH_RS_TXT_X, LAUNCH_RS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rs_Txt, rs, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Arcade") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "1");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "2");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "JOY");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "COIN");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "3");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "4");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "5");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "6");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "PC") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "ENTER");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "USE");
        snprintf(rDown_Txt,   sizeof(rDown_Txt),   "%s", "RUN");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "MOVE");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "MOVE");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "MAP");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rt_Txt, sizeof(rt_Txt), "%s", "FIRE");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rt = {LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "WPN-");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "WPN+");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }

    //Controls all games use
    Vector2 rDown_Size = MeasureTextEx(Var_GetFontBold(), rDown_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 rDown = {LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y};
    DrawTextEx(Var_GetFontBold(), rDown_Txt, rDown, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

    Vector2 rRight_Size = MeasureTextEx(Var_GetFontBold(), rRight_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 rRight = {LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y};
    DrawTextEx(Var_GetFontBold(), rRight_Txt, rRight, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

    Vector2 home_Size = MeasureTextEx(Var_GetFontBold(), home_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 home = {LAUNCH_HOME_TXT_X, LAUNCH_HOME_TXT_Y};
    DrawTextEx(Var_GetFontBold(), home_Txt, home, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

    Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 d = {LAUNCH_D_TXT_X, LAUNCH_D_TXT_Y};
    DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

    Vector2 start_Size = MeasureTextEx(Var_GetFontBold(), start_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 start = {LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y};
    DrawTextEx(Var_GetFontBold(), start_Txt, start, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

    Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
    DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
}

//Draw controls for external application game
static void UI_DrawControls_Ext(const game_t* game) {
    //Draw controller image
    Rectangle sourceRect = {0.0f, 0.0f, (float)controlsImg.width, (float)controlsImg.height};
    Rectangle destRect = {Var_GetMonitorWidth() / 5, Var_GetMonitorHeight() / 3, Var_GetMonitorWidth() / 5 * 3, Var_GetMonitorHeight() / 5 * 3};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(controlsImg, sourceRect, destRect, origin, 0.0f, WHITE);
    //Initialize the text variables for all the controls
    char rDown_Txt[10] = "", rUp_Txt[10] = "", rLeft_Txt[10] = "", rRight_Txt[10] = "";
    char d_Txt[10] = "", rs_Txt[10] = "", back_Txt[10] = "", start_Txt[10] = "";
    char lt_Txt[10] = "", rt_Txt[10] = "", lb_Txt[10] = "", rb_Txt[10] = "", ls_Txt[10] = "";
    char home_Txt[10] = "PAUSE";

    //Draw circles and rectangles for backgrounds of each control
    DrawCircle(LAUNCH_RIGHT_FACE_DOWN_X, LAUNCH_RIGHT_FACE_DOWN_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_UP_X, LAUNCH_RIGHT_FACE_UP_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_LEFT_X, LAUNCH_RIGHT_FACE_LEFT_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_RIGHT_X, LAUNCH_RIGHT_FACE_RIGHT_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawRectangle(LAUNCH_HOME_RECT_X, LAUNCH_HOME_RECT_Y, LAUNCH_HOME_RECT_W, LAUNCH_HOME_RECT_H, GRAY);
    DrawRectangle(LAUNCH_D_RECT_X, LAUNCH_D_RECT_Y, LAUNCH_D_RECT_W, LAUNCH_D_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RS_RECT_X, LAUNCH_RS_RECT_Y, LAUNCH_RS_RECT_W, LAUNCH_RS_RECT_H, GRAY);
    DrawRectangle(LAUNCH_BACK_RECT_X, LAUNCH_BACK_RECT_Y, LAUNCH_BACK_RECT_W, LAUNCH_BACK_RECT_H, GRAY);
    DrawRectangle(LAUNCH_START_RECT_X, LAUNCH_START_RECT_Y, LAUNCH_START_RECT_W, LAUNCH_START_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LT_RECT_X, LAUNCH_LT_RECT_Y, LAUNCH_LT_RECT_W, LAUNCH_LT_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RT_RECT_X, LAUNCH_RT_RECT_Y, LAUNCH_RT_RECT_W, LAUNCH_RT_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LB_RECT_X, LAUNCH_LB_RECT_Y, LAUNCH_LB_RECT_W, LAUNCH_LB_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RB_RECT_X, LAUNCH_RB_RECT_Y, LAUNCH_RB_RECT_W, LAUNCH_RB_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LS_RECT_X, LAUNCH_LS_RECT_Y, LAUNCH_LS_RECT_W, LAUNCH_LS_RECT_H, GRAY);

    //Set the text variables for the console and draw the non-shared ones
    if (strcmp(game->console, "Nintendo GameCube") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "X");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "O");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(d_Txt,     sizeof(d_Txt),     "%s", "D-PAD");

        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "[]");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "/\\");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L1");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R1");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L2");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lt = {LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);

        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R2");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rt = {LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Nintendo DS") == 0) {
        
    }
    else if (strcmp(game->console, "Sony PlayStation Portable") == 0) {
        
    }
    else if (strcmp(game->console, "Sega Saturn") == 0) {
        
    }
    else if (strcmp(game->console, "Sega Dreamcast") == 0) {
        
    }

    //Controls all games use
    Vector2 rDown_Size = MeasureTextEx(Var_GetFontBold(), rDown_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 rDown = {LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y};
    DrawTextEx(Var_GetFontBold(), rDown_Txt, rDown, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    Vector2 rRight_Size = MeasureTextEx(Var_GetFontBold(), rRight_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 rRight = {LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y};
    DrawTextEx(Var_GetFontBold(), rRight_Txt, rRight, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    Vector2 home_Size = MeasureTextEx(Var_GetFontBold(), home_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 home = {LAUNCH_HOME_TXT_X, LAUNCH_HOME_TXT_Y};
    DrawTextEx(Var_GetFontBold(), home_Txt, home, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 d = {LAUNCH_D_TXT_X, LAUNCH_D_TXT_Y};
    DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    Vector2 start_Size = MeasureTextEx(Var_GetFontBold(), start_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 start = {LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y};
    DrawTextEx(Var_GetFontBold(), start_Txt, start, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
    DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
}

//Draw launching screen
void UI_DrawLaunch(const game_t* game) {
    //Clear background
    ClearBackground(BLACK);
    //Launching text
    Vector2 launchTxt_Size = MeasureTextEx(Var_GetFontRegular(), LAUNCH_TXT, LAUNCH_TXT_SIZE, LAUNCH_SPACE);
    Vector2 launchTxt_Pos = {LAUNCH_TITLE_X, LAUNCH_TITLE_Y};
    DrawTextEx(Var_GetFontRegular(), LAUNCH_TXT, launchTxt_Pos, LAUNCH_TXT_SIZE, LAUNCH_SPACE, WHITE);
    //Name of game
    Vector2 gameTxt_Size = MeasureTextEx(Var_GetFontBold(), game->title, LAUNCH_TXT_SIZE, LAUNCH_SPACE);
    Vector2 gameTxt_Pos = {LAUNCH_GAME_X, LAUNCH_GAME_Y};
    DrawTextEx(Var_GetFontBold(), game->title, gameTxt_Pos, LAUNCH_TXT_SIZE, LAUNCH_SPACE, WHITE);
    char homeTxt[55];
    if (game->libRetro) {
        snprintf(homeTxt, size(homeTxt), LAUNCH_HOME_LIB);
        UI_DrawControls_Lib(game);
    }
    else {
        snprintf(homeTxt, size(homeTxt), LAUNCH_HOME_EXT);
        UI_DrawControls_Ext(game);
    }
    //Home instructions
    Vector2 homeTxt_Size = MeasureTextEx(Var_GetFontRegular(), homeTxt, LAUNCH_TXT_SIZE, LAUNCH_SPACE);
    Vector2 homeTxt_Pos = {LAUNCH_HOME_X, LAUNCH_HOME_Y};
    DrawTextEx(Var_GetFontRegular(), homeTxt, homeTxt_Pos, LAUNCH_TXT_SIZE, LAUNCH_SPACE, WHITE);
    
}

//Display diagnostics on top left
void UI_DrawDispDiag(bool in_game) {
    //If diagnostics are being displayed
    if (Var_GetDiag()) {
        Vector2 diagnosticsResolution = {DISP_DIAG_RES_X, DISP_DIAG_RES_Y};
        DrawTextEx(Var_GetFontRegular(), TextFormat("Resolution:  %d:%d", GetMonitorWidth(0), GetMonitorHeight(0)), diagnosticsResolution, DISP_DIAG_SIZE, 
            DISP_DIAG_SPACE, Var_GetColor3());
        Vector2 diagnosticsFPS = {DISP_DIAG_FPS_X, DISP_DIAG_FPS_Y};
        Vector2 temp = {DISP_DIAG_TEMP_X, DISP_DIAG_TEMP_Y};
        DrawTextEx(Var_GetFontRegular(), TextFormat("CPU Temperature:  %.1f C", Var_GetTemp()), temp, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        Vector2 clock = {DISP_DIAG_CLOCK_X, DISP_DIAG_CLOCK_Y};
        DrawTextEx(Var_GetFontRegular(), TextFormat("CPU Clock Speed:  %d kHz", Var_GetClock()), clock, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        Vector2 frame = {DISP_DIAG_FRAME_X, DISP_DIAG_FRAME_Y};
        DrawTextEx(Var_GetFontRegular(), TextFormat("Frame:  %.1f     Worst: %.1f", Var_GetFrameAvg(), Var_GetFrameWorst()), frame, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        if (in_game) {
            DrawTextEx(Var_GetFontRegular(), TextFormat("Emulator FPS:  %f / %f", GetAndResetVRCBCount(), GetCoreTargetFPS()), diagnosticsFPS, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        }
        else {
            DrawTextEx(Var_GetFontRegular(), TextFormat("FPS:  %d", GetFPS()), diagnosticsFPS, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        }
    }
}

//Draw base of the main screen
static void UI_DrawBase() {
    // Clear background
    ClearBackground(BACKGROUND_CLR);
    //Draw background
    Rectangle sourceRect = {0.0f, 0.0f, (float)Var_GetBackground().width, (float)Var_GetBackground().height};
    Rectangle destRect = {UI_CenterImg_X(BACKGROUND_W, CENTER_X), UI_CenterImg_Y(BACKGROUND_H, Var_GetMonitorHeight() / 2), BACKGROUND_W, BACKGROUND_H};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(Var_GetBackground(), sourceRect, destRect, origin, 0.0f, Fade(WHITE, BACKGROUND_ALPHA));
    //Draw sections
    DrawRectangle(0, 0, Var_GetMonitorWidth(), START_SECTION_Y, Fade(Var_GetColor2(), TOP_ALPHA));
    Vector2 startSection1 = {0, START_SECTION_Y};
    Vector2 endSection1 = {Var_GetMonitorWidth(), START_SECTION_Y};
    DrawLineEx(startSection1, endSection1, THICKNESS_BASE, Var_GetColor3());
    Vector2 startSection2 = {0, END_SECTION_Y};
    Vector2 endSection2 = {Var_GetMonitorWidth(), END_SECTION_Y};
    DrawLineEx(startSection2, endSection2, THICKNESS_BASE, Var_GetColor3());
}

//Change alphas when static
void UI_ChangeAlpha_Static() {
    //Blink the correct objects
    if (alphaSelectTxt_Blink == true) {
        // If less than 1, increment
        if (Var_GetAlphaSelect() < 1.0f) {
            Var_AddAlphaSelect(0.04f * GetFrameTime() * 60.0f);
        }
        //If alpha has reached 1
        else if (Var_GetAlphaSelect() >= 1.0f) {
            alphaSelectTxt_TimeElapsed += GetFrameTime();
            //If enough time has passed, keep blinking
            if (alphaSelectTxt_TimeElapsed >= 1) {
                alphaSelectTxt_Blink = false;
                alphaSelectTxt_TimeElapsed = 0.0f;
            }
        }
    }
    else
    {
        // If less than 1, increment
        if (Var_GetAlphaSelect() > 0.1f) {
            Var_AddAlphaSelect(- 0.04f * GetFrameTime() * 60.0f);
        }
        //If alpha has gotten low enough
        else if (Var_GetAlphaSelect() <= 0.1f) {
            alphaSelectTxt_TimeElapsed += GetFrameTime();
            //If enough time has passed, keep blinking
            if (alphaSelectTxt_TimeElapsed >= 0.1) {
                alphaSelectTxt_Blink = true;
                alphaSelectTxt_TimeElapsed = 0.0f;
            }
        }
    }
    //If less than 1, increment
    if (alphaGames < 1.0f) {
        alphaGames += (0.25f * GetFrameTime() * 60.0f);
    }
    //If greater than 1, bring back to 1
    else if (alphaGames > 1.0f) {
        alphaGames = 1.0f;
    }
    //If less than 1, increment
    if (alphaSelectBox < 1.0f) {
        alphaSelectBox += (0.25f * GetFrameTime() * 60.0f);
    }
    //If greater than 1, bring back to 1
    else if (alphaSelectBox > 1.0f) {
        alphaSelectBox = 1.0f;
    }
}

//Change alphas when scrolling grames
static void UI_ChangeAlpha_ScrollGames() {
    //If less than 0, bring to 0
    if (Var_GetAlphaSelect() < 0.0f) {
        Var_SetAlphaSelect(0.0f);
    }
    //If greater than 0, decrement
    else if (Var_GetAlphaSelect() > 0.0f) {
        Var_AddAlphaSelect(- 0.25f * GetFrameTime() * 60.0f);
    }
    //If less than 0, bring to 0
    if (alphaGames < 0.0f) {
        alphaGames = 0.0f;
    }
    //If greater than 0, decrement
    else if (alphaGames > 0.0f) {
        alphaGames -= (0.25f * GetFrameTime() * 60.0f);
    }
    //If less than 1, increment
    if (alphaSelectBox < 1.0f) {
        alphaSelectBox += (0.25f * GetFrameTime() * 60.0f);
    }
    //If greater than 1, bring to 1
    else if (alphaSelectBox > 1.0f) {
        alphaSelectBox = 1.0f;
    }
}

//Change alphas when scrolling categories
static void UI_ChangeAlpha_ScrollCateg() {
    // If less than 0, bring to 0
    if (Var_GetAlphaSelect() < 0.0f) {
        Var_SetAlphaSelect(0.0f);
    }
    // If greater than 0, decrement
    else if (Var_GetAlphaSelect() > 0.0f) {
        Var_AddAlphaSelect(- 0.25f * GetFrameTime() * 60.0f);
    }
    // If less than 0, bring to 0
    if (alphaGames < 0.0f) {
        alphaGames = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaGames > 0.0f) {
        alphaGames -= (0.25f * GetFrameTime() * 60.0f);
    }
    if (alphaSelectBox > 0.0f) {
        alphaSelectBox -= (0.25f * GetFrameTime() * 60.0f);
    }
    else if (alphaSelectBox < 0.0f) {
        alphaSelectBox = 0.0f;
    }
    // If greater than 1, bring to 1
    if (alphaCategories_In > 1.0f) {
        alphaCategories_In = 1.0f;
    }
    // If less than 1, increment up
    else if (alphaCategories_In < 1.0f) {
        alphaCategories_In += (0.05f * GetFrameTime() * 60.0f);
    }
    // If less than 0, bring to 0
    if (alphaCategories_Out < 0.0f) {
        alphaCategories_Out = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaCategories_Out > 0.0f) {
        alphaCategories_Out -= (0.1f * GetFrameTime() * 60.0f);
    }
}

//Draw the cateogires when they are static
static void UI_DrawCateg_Static() {
    //Draw center category
    Vector2 centerCateg = {CENTER_CATEG_X, CENTER_CATEG_Y};
    DrawTextEx(Var_GetFontBold(), Categories_GetDisplayed(2), centerCateg, CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaCategories_Out));
    //Draw left category
    Vector2 left1Categ = {LEFT1_CATEG_X, SIDE_CATEG_Y};
    DrawTextEx(Var_GetFontBold(), Categories_GetDisplayed(1), left1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaCategories_Out));
    //Draw right category
    Vector2 right1Categ = {RIGHT1_CATEG_X, SIDE_CATEG_Y};
    DrawTextEx(Var_GetFontBold(), Categories_GetDisplayed(3), right1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaCategories_Out));
}

//Draw the animation of scrolling categories
static void UI_DrawCateg_Scroll() {
    //If scrolling to the right
    if (Var_GetScrollCateg() == SCROLL_RIGHT) {
        //If images close enough to final spot
        if (fabsf(Games_GetNew(2)->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD) {
            //Update displayed categories
            Categories_ScrollRight();
            Games_UpdateIndexes(Categories_GetDisplayed(2));
            Games_Refresh();
            //Draw categories normally
            UI_DrawCateg_Static();
            //If not still scrolling, stop scrolling
            if ((!IsKeyDown(KEY_D) && !RB_DOWN) || currentUIState == OPTIONS) {
                Var_SetScrollCateg(SCROLL_NO);
            }
            //If still scrolling, keep going
            else {
                Games_UpdateNewIndexes(RIGHT);
                Games_NewRefresh();
                UI_ResetCoords_Categ();
            }
            alphaCategories_Out = 1.0f;
            alphaCategories_In = 0.0f;
            UI_ResetCoords_Games();
            return;
        }
        //Update x coordinates for new games
        Games_GetNew(0)->x = Lerp(Games_GetNew(0)->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(1)->x = Lerp(Games_GetNew(1)->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(2)->x = Lerp(Games_GetNew(2)->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(3)->x = Lerp(Games_GetNew(3)->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(4)->x = Lerp(Games_GetNew(4)->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        //Update x coordinates for old games
        Games_GetDisplayed(1)->x = Lerp(Games_GetDisplayed(1)->x, LEFT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(2)->x = Lerp(Games_GetDisplayed(2)->x, LEFT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(3)->x = Lerp(Games_GetDisplayed(3)->x, CENTER_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(4)->x = Lerp(Games_GetDisplayed(4)->x, RIGHT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(5)->x = Lerp(Games_GetDisplayed(5)->x, RIGHT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
    }
    //If scrolling to the left
    else {
        //If images close enough to final spot
        if (fabsf(Games_GetNew(2)->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD) {
            //Update displayed categories
            Categories_ScrollLeft();
            Games_UpdateIndexes(Categories_GetDisplayed(2));
            Games_Refresh();
            //Draw categories normally
            UI_DrawCateg_Static();
            //If not still scrolling, stop scrolling
            if ((!IsKeyDown(KEY_A) && !LB_DOWN) || currentUIState == OPTIONS) {
                Var_SetScrollCateg(SCROLL_NO);
            }
            //If still scrolling, keep going
            else {
                Games_UpdateNewIndexes(LEFT);
                Games_NewRefresh();
                UI_ResetCoords_Categ();
            }
            alphaCategories_Out = 1.0f;
            alphaCategories_In = 0.0f;
            //Reset values for games
            UI_ResetCoords_Games();
            return;
        }
        // Update x coordinates for new games
        Games_GetNew(0)->x = Lerp(Games_GetNew(0)->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(1)->x = Lerp(Games_GetNew(1)->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(2)->x = Lerp(Games_GetNew(2)->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(3)->x = Lerp(Games_GetNew(3)->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(4)->x = Lerp(Games_GetNew(4)->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        // Update x coordinates for old games
        Games_GetDisplayed(1)->x = Lerp(Games_GetDisplayed(1)->x, LEFT2_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(2)->x = Lerp(Games_GetDisplayed(2)->x, LEFT1_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(3)->x = Lerp(Games_GetDisplayed(3)->x, CENTER_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(4)->x = Lerp(Games_GetDisplayed(4)->x, RIGHT1_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(5)->x = Lerp(Games_GetDisplayed(5)->x, RIGHT2_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
    }
}

//Draw a game cover
static void UI_DrawGame(int i, float alpha, bool scroll) {
    //If scrolling
    if (scroll) {
        //Update values of image to draw
        img_Y = Games_GetNew(i)->y;
        img_W = Games_GetNew(i)->w;
        img_H = Games_GetNew(i)->h;
        img_X = Games_GetNew(i)->x;
        img = Games_GetNew(i)->cover;
        //Draw image
        UI_DrawImage(alpha);
        //Draw outline
        DrawRectangleLinesEx(
            (Rectangle){
                img_X - THICKNESS_GAME_OTHER,
                img_Y - THICKNESS_GAME_OTHER,
                img_W + (2 * THICKNESS_GAME_OTHER),
                img_H + (2 * THICKNESS_GAME_OTHER)},
            THICKNESS_GAME_OTHER,
            Fade(GRAY, alphaCategories_In));
    }
    //If not scrolling
    else {
        //Update values of image to draw
        img_Y = Games_GetDisplayed(i)->y;
        img_W = Games_GetDisplayed(i)->w;
        img_H = Games_GetDisplayed(i)->h;
        img_X = Games_GetDisplayed(i)->x;
        img = Games_GetDisplayed(i)->cover;
        //Draw image
        UI_DrawImage(alpha);
        //Draw outline
        DrawRectangleLinesEx(
            (Rectangle){
                img_X - THICKNESS_GAME_OTHER,
                img_Y - THICKNESS_GAME_OTHER,
                img_W + (2 * THICKNESS_GAME_OTHER),
                img_H + (2 * THICKNESS_GAME_OTHER)},
            THICKNESS_GAME_OTHER,
            Fade(GRAY, alphaCategories_Out));
    }
}

//Draw the games when scrolling to the left
static void UI_DrawGames_Left() {
    UI_DrawGame(0, 1.0f, false);
    UI_DrawGame(1, 1.0f, false);
    UI_DrawGame(5, 1.0f, false);
    UI_DrawGame(4, 1.0f, false);
    UI_DrawGame(3, 1.0f, false);
    UI_DrawGame(2, 1.0f, false);
}

//Draw the games when scrolling to the right
static void UI_DrawGames_Right() {
    UI_DrawGame(6, 1.0f, false);
    UI_DrawGame(1, 1.0f, false);
    UI_DrawGame(5, 1.0f, false);
    UI_DrawGame(2, 1.0f, false);
    UI_DrawGame(3, 1.0f, false);
    UI_DrawGame(4, 1.0f, false);
}

//Draw games when not scrolling
static void UI_DrawGames_Normal() {
    //Draw new games
    if (Var_GetScrollCateg() != SCROLL_NO) {
        UI_DrawGame(0, alphaCategories_In, true);
        UI_DrawGame(4, alphaCategories_In, true);
        UI_DrawGame(1, alphaCategories_In, true);
        UI_DrawGame(3, alphaCategories_In, true);
        UI_DrawGame(2, alphaCategories_In, true);
    }
    UI_DrawGame(1, alphaCategories_Out, false);
    UI_DrawGame(5, alphaCategories_Out, false);
    UI_DrawGame(2, alphaCategories_Out, false);
    UI_DrawGame(4, alphaCategories_Out, false);
    UI_DrawGame(3, alphaCategories_Out, false);

    // Draw outline for selected game
    DrawRectangleLinesEx(
        (Rectangle){
            img_X - THICKNESS_GAME_SELECT,
            img_Y - THICKNESS_GAME_SELECT,
            img_W + (2 * THICKNESS_GAME_SELECT),
            img_H + (2 * THICKNESS_GAME_SELECT)},
        THICKNESS_GAME_SELECT,
        Fade(Var_GetColor1(), alphaGames));
}

//Function for drawing the animation of scrolling games
static void UI_DrawGames_Scroll() {
    //If scrolling to the right
    if (Var_GetScrollGames() == SCROLL_RIGHT) {
        //Draw the games
        UI_DrawGames_Right();
        //If images close enough to final spot
        if (fabsf(Games_GetDisplayed(4)->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD) {
            if ((!IsKeyDown(KEY_RIGHT) && !LS_RIGHT) || currentUIState == OPTIONS) {
                Var_SetScrollGames(SCROLL_NO);
            }
            //Update displayed games
            Games_ScrollRight();
            //Reset game values
            UI_ResetCoords_Games();
            return;
        }
        //Update x coordinates
        Games_GetDisplayed(1)->x = Lerp(Games_GetDisplayed(1)->x, LEFT3_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->x = Lerp(Games_GetDisplayed(2)->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->x = Lerp(Games_GetDisplayed(3)->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->x = Lerp(Games_GetDisplayed(4)->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->x = Lerp(Games_GetDisplayed(5)->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(6)->x = Lerp(Games_GetDisplayed(6)->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        //Update y coordinates
        Games_GetDisplayed(1)->y = Lerp(Games_GetDisplayed(1)->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->y = Lerp(Games_GetDisplayed(2)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->y = Lerp(Games_GetDisplayed(3)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->y = Lerp(Games_GetDisplayed(4)->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->y = Lerp(Games_GetDisplayed(5)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(6)->y = Lerp(Games_GetDisplayed(6)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        //Update width values
        Games_GetDisplayed(1)->w = Lerp(Games_GetDisplayed(1)->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->w = Lerp(Games_GetDisplayed(2)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->w = Lerp(Games_GetDisplayed(3)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->w = Lerp(Games_GetDisplayed(4)->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->w = Lerp(Games_GetDisplayed(5)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(6)->w = Lerp(Games_GetDisplayed(6)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        //Update height values
        Games_GetDisplayed(1)->h = Lerp(Games_GetDisplayed(1)->h, SIDE3_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->h = Lerp(Games_GetDisplayed(2)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->h = Lerp(Games_GetDisplayed(3)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->h = Lerp(Games_GetDisplayed(4)->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->h = Lerp(Games_GetDisplayed(5)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(6)->h = Lerp(Games_GetDisplayed(6)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
    }
    //If scrolling to the left
    else if (Var_GetScrollGames() == SCROLL_LEFT) {
        //Draw the games
        UI_DrawGames_Left();
        //If images close enough to final spot
        if (fabsf(Games_GetDisplayed(2)->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD) {
            if ((!IsKeyDown(KEY_LEFT) && !LS_LEFT) || currentUIState == OPTIONS) {
                Var_SetScrollGames(SCROLL_NO);
            }
            //Update displayed games
            Games_ScrollLeft();
            //Reset game values
            UI_ResetCoords_Games();
            return;
        }
        //Update x coordinates
        Games_GetDisplayed(0)->x = Lerp(Games_GetDisplayed(0)->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(1)->x = Lerp(Games_GetDisplayed(1)->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->x = Lerp(Games_GetDisplayed(2)->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->x = Lerp(Games_GetDisplayed(3)->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->x = Lerp(Games_GetDisplayed(4)->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->x = Lerp(Games_GetDisplayed(5)->x, RIGHT3_GAME_X, SCROLL_GAMES_SPEED);
        //Update y coordinates
        Games_GetDisplayed(0)->y = Lerp(Games_GetDisplayed(0)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(1)->y = Lerp(Games_GetDisplayed(1)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->y = Lerp(Games_GetDisplayed(2)->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->y = Lerp(Games_GetDisplayed(3)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->y = Lerp(Games_GetDisplayed(4)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->y = Lerp(Games_GetDisplayed(5)->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        //Update width values
        Games_GetDisplayed(0)->w = Lerp(Games_GetDisplayed(0)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(1)->w = Lerp(Games_GetDisplayed(1)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->w = Lerp(Games_GetDisplayed(2)->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->w = Lerp(Games_GetDisplayed(3)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->w = Lerp(Games_GetDisplayed(4)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->w = Lerp(Games_GetDisplayed(5)->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        //Update height values
        Games_GetDisplayed(0)->h = Lerp(Games_GetDisplayed(0)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(1)->h = Lerp(Games_GetDisplayed(1)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->h = Lerp(Games_GetDisplayed(2)->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->h = Lerp(Games_GetDisplayed(3)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->h = Lerp(Games_GetDisplayed(4)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->h = Lerp(Games_GetDisplayed(5)->h, SIDE3_GAME_H, SCROLL_GAMES_SPEED);
    }
}

//Tick function for the main UI
void UI_Tick(ConsoleState* currentConsoleState) {

    // Transition
    switch (currentUIState) {
    case NORMAL:
        //If not scrolling games to left, not scrolling categories, and the right input is pressed
        if (Var_GetScrollGames() != SCROLL_LEFT && Var_GetScrollCateg() == SCROLL_NO && (IsKeyDown(KEY_RIGHT) || LS_RIGHT)) {
            //Scroll games to the right
            Var_SetScrollGames(SCROLL_RIGHT);
            currentUIState = SCROLL_GAMES;
        }
        //If not scrolling games to right, not scrolling categories, and the right input is pressed
        else if (Var_GetScrollGames() != SCROLL_RIGHT && Var_GetScrollCateg() == SCROLL_NO && (IsKeyDown(KEY_LEFT) || LS_LEFT)) {
            //Scroll games to the left
            Var_SetScrollGames(SCROLL_LEFT);
            currentUIState = SCROLL_GAMES;
        }
        //If not scrolling categories to left and right input is pressed
        if (Var_GetScrollCateg() != SCROLL_LEFT && (IsKeyDown(KEY_D) || RB_DOWN)) {
            //If not already scrolling cateogries
            if (Var_GetScrollCateg() == SCROLL_NO) {
                //Scroll cateogries to right and don't scroll games
                Var_SetScrollCateg(SCROLL_RIGHT);
                Var_SetScrollGames(SCROLL_NO);
                currentUIState = SCROLL_CATEGORIES;
                //Get new games ready and reset the coordinates
                Games_UpdateNewIndexes(RIGHT);
                Games_NewRefresh();
                UI_ResetCoords_Categ();
            }
        }
        //If not scrolling categories to right and right input is pressed
        else if (Var_GetScrollCateg() != SCROLL_RIGHT && (IsKeyDown(KEY_A) || LB_DOWN)) {
            //If not already scrolling cateogries
            if (Var_GetScrollCateg() == SCROLL_NO) {
                //Scroll cateogries to right and don't scroll games
                Var_SetScrollCateg(SCROLL_LEFT);
                Var_SetScrollGames(SCROLL_NO);
                currentUIState = SCROLL_CATEGORIES;
                //Get new games ready and reset the coordinates
                Games_UpdateNewIndexes(LEFT);
                Games_NewRefresh();
                UI_ResetCoords_Categ();
            }
        }
        if (IsKeyPressed(KEY_TAB) || START_PRESS) {
            UIPause_Init();
            currentUIState = OPTIONS;
        }
        break;

    case SCROLL_GAMES:
        if (Var_GetScrollGames() == SCROLL_NO) {
            currentUIState = NORMAL;
            alphaSelectTxt_Blink = true;
            alphaSelectTxt_TimeElapsed = 0.0f;
        }
        if (IsKeyPressed(KEY_TAB) || START_PRESS) {
            UIPause_Init();
            currentUIState = OPTIONS;
        }
        if (IsKeyPressed(KEY_A) || LB_PRESS) {
            Var_SetScrollGames(SCROLL_NO);
            //Update displayed games
            Games_ScrollLeft();
            //Reset game values
            UI_ResetCoords_Games();
            Var_SetScrollCateg(SCROLL_LEFT);
            currentUIState = SCROLL_CATEGORIES;
            Games_UpdateNewIndexes(LEFT);
            Games_NewRefresh();
            UI_ResetCoords_Categ();
        }
        else if (IsKeyPressed(KEY_D) || RB_PRESS) {
            Var_SetScrollGames(SCROLL_NO);
            //Update displayed games
            Games_ScrollRight();
            //Reset game values
            UI_ResetCoords_Games();
            Var_SetScrollCateg(SCROLL_RIGHT);
            currentUIState = SCROLL_CATEGORIES;
            Games_UpdateNewIndexes(RIGHT);
            Games_NewRefresh();
            UI_ResetCoords_Categ();
        }
        break;

    case SCROLL_CATEGORIES:
        if (Var_GetScrollCateg() == SCROLL_NO) {
            currentUIState = NORMAL;
            alphaSelectTxt_Blink = true;
            alphaSelectTxt_TimeElapsed = 0.0f;
        }
        if (IsKeyPressed(KEY_TAB) || START_PRESS) {
            UIPause_Init();
            currentUIState = OPTIONS;
        }
        break;

    case OPTIONS:
        if (IsKeyPressed(KEY_TAB) || START_PRESS || ((IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) && !Var_GetDisplayBright() && !Var_GetDisplayTheme())) {
            currentUIState = NORMAL;
        }
        break;
    }

    //Action
    switch (currentUIState) {
    case NORMAL:
        UI_ChangeAlpha_Static();
        UI_DrawBase();
        UI_DrawCateg_Static();
        UI_DrawGames_Normal();
        //Draw bumpers
        UI_DrawTop();
        //Draw bottom section of screen
        UI_DrawBottom();
        UI_DrawDispDiag(false);
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && !Controller_GetWasPressed_A()) {
            *currentConsoleState = STATE_APP_LAUNCHER;
            Controller_SetWasPressed_A(true);
        }
        break;

    case SCROLL_GAMES:
        UI_ChangeAlpha_ScrollGames();
        UI_DrawBase();
        UI_DrawCateg_Static();
        UI_DrawGames_Scroll();
        //Draw outline for selected game
        DrawRectangleLinesEx(
            (Rectangle){
                img_X - THICKNESS_GAME_SELECT,
                img_Y - THICKNESS_GAME_SELECT,
                img_W + (2 * THICKNESS_GAME_SELECT),
                img_H + (2 * THICKNESS_GAME_SELECT)},
            THICKNESS_GAME_SELECT,
            Fade(Var_GetColor1(), alphaGames));
        //Draw bumpers
        UI_DrawTop();
        //Draw bottom section of screen
        UI_DrawBottom();
        UI_DrawDispDiag(false);
        break;

    case SCROLL_CATEGORIES:
        UI_ChangeAlpha_ScrollCateg();
        UI_DrawBase();
        UI_DrawCateg_Scroll();
        UI_DrawGames_Normal();
        //Draw bumpers
        UI_DrawTop();
        //Draw bottom section of screen
        UI_DrawBottom();
        UI_DrawDispDiag(false);
        break;

    case OPTIONS:
        UI_DrawBase();
        if (Var_GetScrollGames() != SCROLL_NO) {
            UI_ChangeAlpha_ScrollGames();
            UI_DrawCateg_Static();
            UI_DrawGames_Scroll();
            //Draw outline for selected game
            DrawRectangleLinesEx(
                (Rectangle){
                    img_X - THICKNESS_GAME_SELECT,
                    img_Y - THICKNESS_GAME_SELECT,
                    img_W + (2 * THICKNESS_GAME_SELECT),
                    img_H + (2 * THICKNESS_GAME_SELECT)},
                THICKNESS_GAME_SELECT,
                Fade(Var_GetColor1(), alphaGames));
        }
        else if (Var_GetScrollCateg() != SCROLL_NO) {
            UI_ChangeAlpha_ScrollCateg();
            UI_DrawCateg_Scroll();
            UI_DrawGames_Normal();
        }
        else {
            UI_ChangeAlpha_Static();
            UI_DrawCateg_Static();
            UI_DrawGames_Normal();
        }
        //Draw bumpers
        UI_DrawTop();
        //Draw bottom section of screen
        UI_DrawBottom();
        UI_DrawDispDiag(false);
        UIPause_Tick(currentConsoleState);
        break;
    }
}

//Init function for the main UI
void UI_Init() {
    //Initialize the alphas
    alphaGames = 1.0f;
    alphaCategories_Out = 1.0f;
    alphaCategories_In = 0.0f;
    alphaSelectBox = 1.0f;
    alphaSelectTxt_TimeElapsed = 0.0f;
    alphaSelectTxt_Blink = true;
    //Reset the coordinates for the games
    UI_ResetCoords_Games();
    currentUIState = NORMAL;
}