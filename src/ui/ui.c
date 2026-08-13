#include "ui.h"
#include "raylib.h"
#include "raymath.h"
#include <string.h>
#include <math.h>
#include "states.h"
#include "categories.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
#include "retro_bridge.h"
#include "erase.h"

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
static Texture2D keyImg;
static Texture2D mouseImg;
static Texture2D arrowsImg;


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
void UI_LoadControlsImgs() {
    controlsImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/controller.png");
    keyImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/keyboard.png");
    mouseImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/mouse.png");
    arrowsImg = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/arrowkeys.png");
}

//Draw controls for libretro game
static void UI_DrawControls_Lib(const game_t* game) {
    //Draw controller image
    Rectangle sourceRect_Controller = {0.0f, 0.0f, (float)controlsImg.width, (float)controlsImg.height};
    Rectangle destRect_Controller = {LAUNCH_CONTROLLER_X, LAUNCH_CONTROLLER_Y, LAUNCH_CONTROLLER_W, LAUNCH_CONTROLLER_H};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(controlsImg, sourceRect_Controller, destRect_Controller, origin, 0.0f, WHITE);
    //Draw keyboard rectangle
    Rectangle rect = {LAUNCH_KEYBOARD_X, LAUNCH_KEYBOARD_Y, LAUNCH_KEYBOARD_W, LAUNCH_KEYBOARD_H};
    DrawRectanglePro(rect, origin, 0.0f, WHITE);
    Vector2 middleLineStart = {LAUNCH_KEYBOARD_X, LAUNCH_CONTROLLER_Y};
    Vector2 middleLineEnd = {LAUNCH_KEYBOARD_X, LAUNCH_CONTROLLER_Y + LAUNCH_CONTROLLER_H};
    DrawLineEx(middleLineStart, middleLineEnd, LAUNCH_CONTROL_OUTLINE_THICKNESS, GRAY);
    //Initialize the text variables for all the controls
    char rDown_Txt[10] = "", rUp_Txt[10] = "", rLeft_Txt[10] = "", rRight_Txt[10] = "";
    char dPad_Txt[10] = "", rs_Txt[10] = "", back_Txt[10] = "", start_Txt[10] = "";
    char lt_Txt[10] = "", rt_Txt[10] = "", lb_Txt[15] = "", rb_Txt[15] = "", ls_Txt[10] = "";
    char home_Txt[10] = "PAUSE";
    char tab_Txt[10] = "", q_Txt[13] = "", w_Txt[10] = "", e_Txt[13] = "", u_Txt[10] = "", i_Txt[10] = "", o_Txt[10] = "";
    char a_Txt[10] = "", s_Txt[10] = "", d_Txt[10] = "", f_Txt[10] = "", j_Txt[10] = "", k_Txt[10] = "", l_Txt[10] = "";
    char enter_Txt[10] = "", lShift_Txt[10] = "", rShift_Txt[10] = "", space_Txt[10] = "", mouse_Txt[10] = "", lClick_Txt[10] = "", arrows_Txt[10] = "";
    //Draw circles and rectangles for backgrounds of each xbox control
    DrawCircle(LAUNCH_RIGHT_FACE_DOWN_X, LAUNCH_RIGHT_FACE_DOWN_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_UP_X, LAUNCH_RIGHT_FACE_UP_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_LEFT_X, LAUNCH_RIGHT_FACE_LEFT_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_RIGHT_X, LAUNCH_RIGHT_FACE_RIGHT_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawRectangle(LAUNCH_HOME_RECT_X, LAUNCH_HOME_RECT_Y, LAUNCH_HOME_RECT_W, LAUNCH_HOME_RECT_H, GRAY);
    DrawRectangle(LAUNCH_DPAD_RECT_X, LAUNCH_DPAD_RECT_Y, LAUNCH_DPAD_RECT_W, LAUNCH_DPAD_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RS_RECT_X, LAUNCH_RS_RECT_Y, LAUNCH_RS_RECT_W, LAUNCH_RS_RECT_H, GRAY);
    DrawRectangle(LAUNCH_BACK_RECT_X, LAUNCH_BACK_RECT_Y, LAUNCH_BACK_RECT_W, LAUNCH_BACK_RECT_H, GRAY);
    DrawRectangle(LAUNCH_START_RECT_X, LAUNCH_START_RECT_Y, LAUNCH_START_RECT_W, LAUNCH_START_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LT_RECT_X, LAUNCH_LT_RECT_Y, LAUNCH_LT_RECT_W, LAUNCH_LT_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RT_RECT_X, LAUNCH_RT_RECT_Y, LAUNCH_RT_RECT_W, LAUNCH_RT_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LB_RECT_X, LAUNCH_LB_RECT_Y, LAUNCH_LB_RECT_W, LAUNCH_LB_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RB_RECT_X, LAUNCH_RB_RECT_Y, LAUNCH_RB_RECT_W, LAUNCH_RB_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LS_RECT_X, LAUNCH_LS_RECT_Y, LAUNCH_LS_RECT_W, LAUNCH_LS_RECT_H, GRAY);
    //Keyboard controls
    Rectangle sourceRect_Key = {0.0f, 0.0f, (float)keyImg.width, (float)keyImg.height};
    Rectangle destRect_Key = {LAUNCH_KEY_X, LAUNCH_KEY_Y, LAUNCH_KEY_W, LAUNCH_KEY_H};
    DrawTexturePro(keyImg, sourceRect_Key, destRect_Key, origin, 0.0f, WHITE);
    Rectangle sourceRect_Mouse = {0.0f, 0.0f, (float)mouseImg.width, (float)mouseImg.height};
    Rectangle destRect_Mouse = {LAUNCH_MOUSE_X, LAUNCH_MOUSE_Y, LAUNCH_MOUSE_W, LAUNCH_MOUSE_H};
    DrawTexturePro(mouseImg, sourceRect_Mouse, destRect_Mouse, origin, 0.0f, WHITE);
    Rectangle sourceRect_Arrow = {0.0f, 0.0f, (float)arrowsImg.width, (float)arrowsImg.height};
    Rectangle destRect_Arrow = {LAUNCH_ARROWS_X, LAUNCH_ARROWS_Y, LAUNCH_ARROWS_W, LAUNCH_ARROWS_H};
    DrawTexturePro(arrowsImg, sourceRect_Arrow, destRect_Arrow, origin, 0.0f, WHITE);
    //1st keyboard row
    Vector2 startTab = {LAUNCH_KEY_TAB_LINE_X, LAUNCH_KEY_TAB_LINE_Y_START};
    Vector2 endTab = {LAUNCH_KEY_TAB_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startTab, endTab, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectTab = {LAUNCH_KEY_TAB_X, LAUNCH_KEY_TAB_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectTab, origin, 0.0f, GRAY);
    Vector2 startQ = {LAUNCH_KEY_Q_LINE_X, LAUNCH_KEY_Q_LINE_Y_START};
    Vector2 endQ = {LAUNCH_KEY_Q_LINE_X, LAUNCH_KEY_LINE_END_TOP2};
    DrawLineEx(startQ, endQ, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectQ = {LAUNCH_KEY_Q_X, LAUNCH_KEY_Q_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectQ, origin, 0.0f, GRAY);
    Vector2 startW = {LAUNCH_KEY_W_LINE_X, LAUNCH_KEY_W_LINE_Y_START};
    Vector2 endW = {LAUNCH_KEY_W_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startW, endW, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectW = {LAUNCH_KEY_W_X, LAUNCH_KEY_W_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectW, origin, 0.0f, GRAY);
    Vector2 startE = {LAUNCH_KEY_E_LINE_X, LAUNCH_KEY_E_LINE_Y_START};
    Vector2 endE = {LAUNCH_KEY_E_LINE_X, LAUNCH_KEY_LINE_END_TOP2};
    DrawLineEx(startE, endE, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectE = {LAUNCH_KEY_E_X, LAUNCH_KEY_E_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectE, origin, 0.0f, GRAY);
    Vector2 startU = {LAUNCH_KEY_U_LINE_X, LAUNCH_KEY_U_LINE_Y_START};
    Vector2 endU = {LAUNCH_KEY_U_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startU, endU, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectU = {LAUNCH_KEY_U_X, LAUNCH_KEY_U_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectU, origin, 0.0f, GRAY);
    Vector2 startI = {LAUNCH_KEY_I_LINE_X, LAUNCH_KEY_I_LINE_Y_START};
    Vector2 endI = {LAUNCH_KEY_I_LINE_X, LAUNCH_KEY_LINE_END_TOP2};
    DrawLineEx(startI, endI, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectI = {LAUNCH_KEY_I_X, LAUNCH_KEY_I_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectI, origin, 0.0f, GRAY);
    Vector2 startO = {LAUNCH_KEY_O_LINE_X, LAUNCH_KEY_O_LINE_Y_START};
    Vector2 endO = {LAUNCH_KEY_O_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startO, endO, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectO = {LAUNCH_KEY_O_X, LAUNCH_KEY_O_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectO, origin, 0.0f, GRAY);
    //2nd keyboard row
    Vector2 startA = {LAUNCH_KEY_A_LINE_X, LAUNCH_KEY_A_LINE_Y_START};
    Vector2 endA = {LAUNCH_KEY_A_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startA, endA, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectA = {LAUNCH_KEY_A_X, LAUNCH_KEY_A_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectA, origin, 0.0f, GRAY);
    Vector2 startS = {LAUNCH_KEY_S_LINE_X, LAUNCH_KEY_S_LINE_Y_START};
    Vector2 endS = {LAUNCH_KEY_S_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startS, endS, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectS = {LAUNCH_KEY_S_X, LAUNCH_KEY_S_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectS, origin, 0.0f, GRAY);
    Vector2 startD = {LAUNCH_KEY_D_LINE_X, LAUNCH_KEY_D_LINE_Y_START};
    Vector2 endD = {LAUNCH_KEY_D_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startD, endD, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectD = {LAUNCH_KEY_D_X, LAUNCH_KEY_D_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectD, origin, 0.0f, GRAY);
    Vector2 startF = {LAUNCH_KEY_F_LINE_X, LAUNCH_KEY_F_LINE_Y_START};
    Vector2 endF = {LAUNCH_KEY_F_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startF, endF, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectF = {LAUNCH_KEY_F_X, LAUNCH_KEY_F_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectF, origin, 0.0f, GRAY);
    Vector2 startJ = {LAUNCH_KEY_J_LINE_X, LAUNCH_KEY_J_LINE_Y_START};
    Vector2 endJ = {LAUNCH_KEY_J_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startJ, endJ, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectJ = {LAUNCH_KEY_J_X, LAUNCH_KEY_J_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectJ, origin, 0.0f, GRAY);
    Vector2 startK = {LAUNCH_KEY_K_LINE_X, LAUNCH_KEY_K_LINE_Y_START};
    Vector2 endK = {LAUNCH_KEY_K_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startK, endK, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectK = {LAUNCH_KEY_K_X, LAUNCH_KEY_K_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectK, origin, 0.0f, GRAY);
    Vector2 startL = {LAUNCH_KEY_L_LINE_X, LAUNCH_KEY_L_LINE_Y_START};
    Vector2 endL = {LAUNCH_KEY_L_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startL, endL, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectL = {LAUNCH_KEY_L_X, LAUNCH_KEY_L_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectL, origin, 0.0f, GRAY);
    //Other keys
    Vector2 startEnter = {LAUNCH_KEY_ENTER_LINE_X, LAUNCH_KEY_ENTER_LINE_Y_START};
    Vector2 endEnter = {LAUNCH_KEY_ENTER_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startEnter, endEnter, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectEnter = {LAUNCH_KEY_ENTER_X, LAUNCH_KEY_ENTER_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectEnter, origin, 0.0f, GRAY);
    Vector2 startRShift = {LAUNCH_KEY_RSHIFT_LINE_X, LAUNCH_KEY_RSHIFT_LINE_Y_START};
    Vector2 endRShift = {LAUNCH_KEY_RSHIFT_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startRShift, endRShift, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectRShift = {LAUNCH_KEY_RSHIFT_X, LAUNCH_KEY_RSHIFT_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectRShift, origin, 0.0f, GRAY);
    Vector2 startLShift = {LAUNCH_KEY_LSHIFT_LINE_X, LAUNCH_KEY_LSHIFT_LINE_Y_START};
    Vector2 endLShift = {LAUNCH_KEY_LSHIFT_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startLShift, endLShift, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectLShift = {LAUNCH_KEY_LSHIFT_X, LAUNCH_KEY_LSHIFT_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectLShift, origin, 0.0f, GRAY);
    Vector2 startSpace = {LAUNCH_KEY_SPACE_LINE_X, LAUNCH_KEY_SPACE_LINE_Y_START};
    Vector2 endSpace = {LAUNCH_KEY_SPACE_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startSpace, endSpace, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectSpace = {LAUNCH_KEY_SPACE_X, LAUNCH_KEY_SPACE_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectSpace, origin, 0.0f, GRAY);
    //Mouse
    Vector2 startMouse = {LAUNCH_KEY_MOUSE_LINE_X, LAUNCH_KEY_MOUSE_LINE_Y_START};
    Vector2 endMouse = {LAUNCH_KEY_MOUSE_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startMouse, endMouse, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectMouse = {LAUNCH_KEY_MOUSE_X, LAUNCH_KEY_MOUSE_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectMouse, origin, 0.0f, GRAY);
    Vector2 startLClick = {LAUNCH_KEY_LCLICK_LINE_X, LAUNCH_KEY_LCLICK_LINE_Y_START};
    Vector2 endLClick = {LAUNCH_KEY_LCLICK_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startLClick, endLClick, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectLClick = {LAUNCH_KEY_LCLICK_X, LAUNCH_KEY_LCLICK_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectLClick, origin, 0.0f, GRAY);
    //Arrows
    Vector2 startArrows = {LAUNCH_KEY_ARROWS_LINE_X1, LAUNCH_KEY_ARROWS_LINE_Y};
    Vector2 endArrows = {LAUNCH_KEY_ARROWS_LINE_X2, LAUNCH_KEY_ARROWS_LINE_Y};
    DrawLineEx(startArrows, endArrows, LAUNCH_ARROW_THICKNESS, GRAY);
    Rectangle rectArrows = {LAUNCH_KEY_ARROWS_REC_X, LAUNCH_KEY_ARROWS_REC_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectArrows, origin, 0.0f, GRAY);
    //Set the text variables for the console and draw the non-shared ones
    if (strcmp(game->console, "Sega Genesis") == 0 || strcmp(game->console, "Sega CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(back_Txt,  sizeof(back_Txt),  "MODE");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "MODE");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "C");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "C");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 i = {LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "X");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "X");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 u = {LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "Y");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(l_Txt,    sizeof(l_Txt),    "Y");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 l = {LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "Z");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "Z");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 o = {LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sega Game Gear") == 0 || strcmp(game->console, "Sega Master System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "1");
        snprintf(j_Txt,    sizeof(j_Txt),    "1");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "2");
        snprintf(k_Txt,    sizeof(k_Txt),    "2");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
    }
    else if (strcmp(game->console, "TurboGrafx-16") == 0 || strcmp(game->console, "TurboGrafx-CD") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "RUN");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "RUN");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "I");
        snprintf(k_Txt,    sizeof(k_Txt),    "I");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "II");
        snprintf(j_Txt,    sizeof(j_Txt),    "II");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Neo Geo Pocket Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "OPTION");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "OPTION");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "B");
        snprintf(k_Txt,    sizeof(k_Txt),    "B");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "A");
        snprintf(j_Txt,    sizeof(j_Txt),    "A");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "STICK");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "STICK");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "STICK");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "STICK");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
    }
    else if (strcmp(game->console, "Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Game Boy Color") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Atari Lynx") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Super Nintendo Entertainment System") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "Y");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "Y");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 i = {LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "X");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(l_Txt,    sizeof(l_Txt),    "X");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 l = {LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "L");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 u = {LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "R");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 o = {LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Game Boy Advance") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(j_Txt,    sizeof(j_Txt),    "B");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(k_Txt,    sizeof(k_Txt),    "A");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "L");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 u = {LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "R");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 o = {LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sony PlayStation") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "X");
        snprintf(k_Txt,    sizeof(k_Txt),    "X");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "O");
        snprintf(l_Txt,    sizeof(l_Txt),    "O");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 l = {LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "SQR");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(j_Txt,    sizeof(j_Txt),    "SQR");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "TRI");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "TRI");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 i = {LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L1");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "L1");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 u = {LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R1");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "R1");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 o = {LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L2");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lt = {LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(q_Txt,    sizeof(q_Txt),    "L2");
        Vector2 q_Size = MeasureTextEx(Var_GetFontBold(), q_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 q = {LAUNCH_KEY_Q_TXT_X, LAUNCH_KEY_Q_TXT_Y};
        DrawTextEx(Var_GetFontBold(), q_Txt, q, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R2");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rt = {LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(e_Txt,    sizeof(e_Txt),    "R2");
        Vector2 e_Size = MeasureTextEx(Var_GetFontBold(), e_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 e = {LAUNCH_KEY_E_TXT_X, LAUNCH_KEY_E_TXT_Y};
        DrawTextEx(Var_GetFontBold(), e_Txt, e, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Nintendo 64") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(u_Txt,    sizeof(u_Txt),    "B");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 u = {LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "A");
        snprintf(o_Txt,    sizeof(o_Txt),    "A");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 o = {LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "STICK");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "STICK");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "STICK");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "STICK");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(arrows_Txt,    sizeof(arrows_Txt),    "D-PAD");
        Vector2 arrows_Size = MeasureTextEx(Var_GetFontBold(), arrows_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 arrows = {LAUNCH_KEY_ARROWS_TXT_X, LAUNCH_KEY_ARROWS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), arrows_Txt, arrows, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(q_Txt,    sizeof(q_Txt),    "L");
        Vector2 q_Size = MeasureTextEx(Var_GetFontBold(), q_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 q = {LAUNCH_KEY_Q_TXT_X, LAUNCH_KEY_Q_TXT_Y};
        DrawTextEx(Var_GetFontBold(), q_Txt, q, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(e_Txt,    sizeof(e_Txt),    "R");
        Vector2 e_Size = MeasureTextEx(Var_GetFontBold(), e_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 e = {LAUNCH_KEY_E_TXT_X, LAUNCH_KEY_E_TXT_Y};
        DrawTextEx(Var_GetFontBold(), e_Txt, e, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "Z");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lt = {LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(space_Txt,    sizeof(space_Txt),    "Z");
        Vector2 space_Size = MeasureTextEx(Var_GetFontBold(), space_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 space = {LAUNCH_KEY_SPACE_TXT_X, LAUNCH_KEY_SPACE_TXT_Y};
        DrawTextEx(Var_GetFontBold(), space_Txt, space, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rs_Txt,    sizeof(rs_Txt),    "%s", "C");
        Vector2 rs_Size = MeasureTextEx(Var_GetFontBold(), rs_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rs = {LAUNCH_RS_TXT_X, LAUNCH_RS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rs_Txt, rs, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(j_Txt,    sizeof(j_Txt),    "C");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(k_Txt,    sizeof(k_Txt),    "C");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "C");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 i = {LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(l_Txt,    sizeof(l_Txt),    "C");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 l = {LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Arcade") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "1");
        snprintf(k_Txt,    sizeof(k_Txt),    "1");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "2");
        snprintf(l_Txt,    sizeof(l_Txt),    "2");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 l = {LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "STICK");
        snprintf(a_Txt,    sizeof(a_Txt),    "STICK");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "STICK");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "STICK");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "STICK");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "COIN");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "COIN");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "3");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(j_Txt,    sizeof(j_Txt),    "3");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "4");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "4");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 i = {LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "5");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "5");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 u = {LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "6");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "6");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 o = {LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "PC") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "ENTER");
        snprintf(enter_Txt,    sizeof(enter_Txt),    "ENTER");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt,sizeof(rDown_Txt),"%s", "USE");
        snprintf(space_Txt,    sizeof(space_Txt),    "USE");
        Vector2 space_Size = MeasureTextEx(Var_GetFontBold(), space_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 space = {LAUNCH_KEY_SPACE_TXT_X, LAUNCH_KEY_SPACE_TXT_Y};
        DrawTextEx(Var_GetFontBold(), space_Txt, space, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "MOVE/RUN");
        Vector2 ls_Size = MeasureTextEx(Var_GetFontBold(), ls_Txt, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE);
        Vector2 ls = {LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), ls_Txt, ls, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "STRAFE");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "MOVE");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "STRAFE");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "MOVE");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE_DOOM1, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(lShift_Txt,    sizeof(lShift_Txt),    "RUN");
        Vector2 lShift_Size = MeasureTextEx(Var_GetFontBold(), lShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lShift = {LAUNCH_KEY_LSHIFT_TXT_X, LAUNCH_KEY_LSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lShift_Txt, lShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "MAP");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(tab_Txt,    sizeof(tab_Txt),    "MAP");
        Vector2 tab_Size = MeasureTextEx(Var_GetFontBold(), tab_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 tab = {LAUNCH_KEY_TAB_TXT_X, LAUNCH_KEY_TAB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), tab_Txt, tab, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rt_Txt, sizeof(rt_Txt), "%s", "FIRE");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rt = {LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lClick_Txt,    sizeof(lClick_Txt),    "FIRE");
        Vector2 lClick_Size = MeasureTextEx(Var_GetFontBold(), lClick_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lClick = {LAUNCH_KEY_LCLICK_TXT_X, LAUNCH_KEY_LCLICK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lClick_Txt, lClick, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(f_Txt,    sizeof(f_Txt),    "FIRE");
        Vector2 f_Size = MeasureTextEx(Var_GetFontBold(), f_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 f = {LAUNCH_KEY_F_TXT_X, LAUNCH_KEY_F_TXT_Y};
        DrawTextEx(Var_GetFontBold(), f_Txt, f, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "SWITCH GUNS");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE_DOOM2, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE_DOOM2, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(q_Txt,    sizeof(q_Txt),    "SWITCH GUNS");
        Vector2 q_Size = MeasureTextEx(Var_GetFontBold(), q_Txt, LAUNCH_CONTROL_SIZE_DOOM2, LAUNCH_CONTROL_SPACE);
        Vector2 q = {LAUNCH_KEY_Q_TXT_X, LAUNCH_KEY_Q_TXT_Y};
        DrawTextEx(Var_GetFontBold(), q_Txt, q, LAUNCH_CONTROL_SIZE_DOOM2, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "SWITCH GUNS");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE_DOOM2, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE_DOOM2, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(e_Txt,    sizeof(e_Txt),    "SWITCH GUNS");
        Vector2 e_Size = MeasureTextEx(Var_GetFontBold(), e_Txt, LAUNCH_CONTROL_SIZE_DOOM2, LAUNCH_CONTROL_SPACE);
        Vector2 e = {LAUNCH_KEY_E_TXT_X, LAUNCH_KEY_E_TXT_Y};
        DrawTextEx(Var_GetFontBold(), e_Txt, e, LAUNCH_CONTROL_SIZE_DOOM2, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rs_Txt,    sizeof(rs_Txt),    "%s", "TURN");
        Vector2 rs_Size = MeasureTextEx(Var_GetFontBold(), rs_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rs = {LAUNCH_RS_TXT_X, LAUNCH_RS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rs_Txt, rs, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(mouse_Txt,    sizeof(mouse_Txt),    "TURN");
        Vector2 mouse_Size = MeasureTextEx(Var_GetFontBold(), mouse_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 mouse = {LAUNCH_KEY_MOUSE_TXT_X, LAUNCH_KEY_MOUSE_TXT_Y};
        DrawTextEx(Var_GetFontBold(), mouse_Txt, mouse, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
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
    Vector2 dPad_Size = MeasureTextEx(Var_GetFontBold(), dPad_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 dPad = {LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y};
    DrawTextEx(Var_GetFontBold(), dPad_Txt, dPad, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    Vector2 start_Size = MeasureTextEx(Var_GetFontBold(), start_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 start = {LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y};
    DrawTextEx(Var_GetFontBold(), start_Txt, start, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    //Draw outline
    Rectangle outline = {LAUNCH_CONTROLLER_X, LAUNCH_CONTROLLER_Y, LAUNCH_CONTROLLER_W * 2, LAUNCH_CONTROLLER_H};
    DrawRectangleLinesEx(outline, LAUNCH_CONTROL_OUTLINE_THICKNESS, GRAY);
}

//Draw controls for external application game
static void UI_DrawControls_Ext(const game_t* game) {
    //Draw controller image
    Rectangle sourceRect_Controller = {0.0f, 0.0f, (float)controlsImg.width, (float)controlsImg.height};
    Rectangle destRect_Controller = {LAUNCH_CONTROLLER_X, LAUNCH_CONTROLLER_Y, LAUNCH_CONTROLLER_W, LAUNCH_CONTROLLER_H};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(controlsImg, sourceRect_Controller, destRect_Controller, origin, 0.0f, WHITE);
    //Draw keyboard rectangle
    Rectangle rect = {LAUNCH_KEYBOARD_X, LAUNCH_KEYBOARD_Y, LAUNCH_KEYBOARD_W, LAUNCH_KEYBOARD_H};
    DrawRectanglePro(rect, origin, 0.0f, WHITE);
    Vector2 middleLineStart = {LAUNCH_KEYBOARD_X, LAUNCH_CONTROLLER_Y};
    Vector2 middleLineEnd = {LAUNCH_KEYBOARD_X, LAUNCH_CONTROLLER_Y + LAUNCH_CONTROLLER_H};
    DrawLineEx(middleLineStart, middleLineEnd, LAUNCH_CONTROL_OUTLINE_THICKNESS, GRAY);
    //Initialize the text variables for all the controls
    char rDown_Txt[10] = "", rUp_Txt[10] = "", rLeft_Txt[10] = "", rRight_Txt[10] = "";
    char dPad_Txt[10] = "", rs_Txt[10] = "", back_Txt[10] = "", start_Txt[10] = "";
    char lt_Txt[10] = "", rt_Txt[10] = "", lb_Txt[15] = "", rb_Txt[15] = "", ls_Txt[10] = "";
    char home_Txt[10] = "EXIT";
    char tab_Txt[10] = "",  q_Txt[13] = "",  e_Txt[13] = "", w_Txt[10] = "", u_Txt[10] = "", i_Txt[10] = "", o_Txt[10] = "";
    char a_Txt[10] = "", s_Txt[10] = "", d_Txt[10] = "", j_Txt[10] = "", k_Txt[10] = "", l_Txt[10] = "";
    char enter_Txt[10] = "", rShift_Txt[10] = "", space_Txt[15] = "", mouse_Txt[10] = "", lClick_Txt[10] = "", arrows_Txt[10] = "";
    //Draw circles and rectangles for backgrounds of each xbox control
    DrawCircle(LAUNCH_RIGHT_FACE_DOWN_X, LAUNCH_RIGHT_FACE_DOWN_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_UP_X, LAUNCH_RIGHT_FACE_UP_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_LEFT_X, LAUNCH_RIGHT_FACE_LEFT_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawCircle(LAUNCH_RIGHT_FACE_RIGHT_X, LAUNCH_RIGHT_FACE_RIGHT_Y, LAUNCH_CONTROL_RADIUS, GRAY);
    DrawRectangle(LAUNCH_HOME_RECT_X, LAUNCH_HOME_RECT_Y, LAUNCH_HOME_RECT_W, LAUNCH_HOME_RECT_H, GRAY);
    DrawRectangle(LAUNCH_DPAD_RECT_X, LAUNCH_DPAD_RECT_Y, LAUNCH_DPAD_RECT_W, LAUNCH_DPAD_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RS_RECT_X, LAUNCH_RS_RECT_Y, LAUNCH_RS_RECT_W, LAUNCH_RS_RECT_H, GRAY);
    DrawRectangle(LAUNCH_BACK_RECT_X, LAUNCH_BACK_RECT_Y, LAUNCH_BACK_RECT_W, LAUNCH_BACK_RECT_H, GRAY);
    DrawRectangle(LAUNCH_START_RECT_X, LAUNCH_START_RECT_Y, LAUNCH_START_RECT_W, LAUNCH_START_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LT_RECT_X, LAUNCH_LT_RECT_Y, LAUNCH_LT_RECT_W, LAUNCH_LT_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RT_RECT_X, LAUNCH_RT_RECT_Y, LAUNCH_RT_RECT_W, LAUNCH_RT_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LB_RECT_X, LAUNCH_LB_RECT_Y, LAUNCH_LB_RECT_W, LAUNCH_LB_RECT_H, GRAY);
    DrawRectangle(LAUNCH_RB_RECT_X, LAUNCH_RB_RECT_Y, LAUNCH_RB_RECT_W, LAUNCH_RB_RECT_H, GRAY);
    DrawRectangle(LAUNCH_LS_RECT_X, LAUNCH_LS_RECT_Y, LAUNCH_LS_RECT_W, LAUNCH_LS_RECT_H, GRAY);
    //Keyboard controls
    Rectangle sourceRect_Key = {0.0f, 0.0f, (float)keyImg.width, (float)keyImg.height};
    Rectangle destRect_Key = {LAUNCH_KEY_X, LAUNCH_KEY_Y, LAUNCH_KEY_W, LAUNCH_KEY_H};
    DrawTexturePro(keyImg, sourceRect_Key, destRect_Key, origin, 0.0f, WHITE);
    Rectangle sourceRect_Mouse = {0.0f, 0.0f, (float)mouseImg.width, (float)mouseImg.height};
    Rectangle destRect_Mouse = {LAUNCH_MOUSE_X, LAUNCH_MOUSE_Y, LAUNCH_MOUSE_W, LAUNCH_MOUSE_H};
    DrawTexturePro(mouseImg, sourceRect_Mouse, destRect_Mouse, origin, 0.0f, WHITE);
    Rectangle sourceRect_Arrow = {0.0f, 0.0f, (float)arrowsImg.width, (float)arrowsImg.height};
    Rectangle destRect_Arrow = {LAUNCH_ARROWS_X, LAUNCH_ARROWS_Y, LAUNCH_ARROWS_W, LAUNCH_ARROWS_H};
    DrawTexturePro(arrowsImg, sourceRect_Arrow, destRect_Arrow, origin, 0.0f, WHITE);
    //1st keyboard row
    Vector2 startTab = {LAUNCH_KEY_TAB_LINE_X, LAUNCH_KEY_TAB_LINE_Y_START};
    Vector2 endTab = {LAUNCH_KEY_TAB_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startTab, endTab, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectTab = {LAUNCH_KEY_TAB_X, LAUNCH_KEY_TAB_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectTab, origin, 0.0f, GRAY);
    Vector2 startQ = {LAUNCH_KEY_Q_LINE_X, LAUNCH_KEY_Q_LINE_Y_START};
    Vector2 endQ = {LAUNCH_KEY_Q_LINE_X, LAUNCH_KEY_LINE_END_TOP2};
    DrawLineEx(startQ, endQ, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectQ = {LAUNCH_KEY_Q_X, LAUNCH_KEY_Q_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectQ, origin, 0.0f, GRAY);
    Vector2 startW = {LAUNCH_KEY_W_LINE_X, LAUNCH_KEY_W_LINE_Y_START};
    Vector2 endW = {LAUNCH_KEY_W_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startW, endW, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectW = {LAUNCH_KEY_W_X, LAUNCH_KEY_W_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectW, origin, 0.0f, GRAY);
    Vector2 startE = {LAUNCH_KEY_E_LINE_X, LAUNCH_KEY_E_LINE_Y_START};
    Vector2 endE = {LAUNCH_KEY_E_LINE_X, LAUNCH_KEY_LINE_END_TOP2};
    DrawLineEx(startE, endE, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectE = {LAUNCH_KEY_E_X, LAUNCH_KEY_E_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectE, origin, 0.0f, GRAY);
    Vector2 startU = {LAUNCH_KEY_U_LINE_X, LAUNCH_KEY_U_LINE_Y_START};
    Vector2 endU = {LAUNCH_KEY_U_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startU, endU, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectU = {LAUNCH_KEY_U_X, LAUNCH_KEY_U_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectU, origin, 0.0f, GRAY);
    Vector2 startI = {LAUNCH_KEY_I_LINE_X, LAUNCH_KEY_I_LINE_Y_START};
    Vector2 endI = {LAUNCH_KEY_I_LINE_X, LAUNCH_KEY_LINE_END_TOP2};
    DrawLineEx(startI, endI, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectI = {LAUNCH_KEY_I_X, LAUNCH_KEY_I_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectI, origin, 0.0f, GRAY);
    Vector2 startO = {LAUNCH_KEY_O_LINE_X, LAUNCH_KEY_O_LINE_Y_START};
    Vector2 endO = {LAUNCH_KEY_O_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startO, endO, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectO = {LAUNCH_KEY_O_X, LAUNCH_KEY_O_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectO, origin, 0.0f, GRAY);
    //2nd keyboard row
    Vector2 startA = {LAUNCH_KEY_A_LINE_X, LAUNCH_KEY_A_LINE_Y_START};
    Vector2 endA = {LAUNCH_KEY_A_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startA, endA, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectA = {LAUNCH_KEY_A_X, LAUNCH_KEY_A_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectA, origin, 0.0f, GRAY);
    Vector2 startS = {LAUNCH_KEY_S_LINE_X, LAUNCH_KEY_S_LINE_Y_START};
    Vector2 endS = {LAUNCH_KEY_S_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startS, endS, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectS = {LAUNCH_KEY_S_X, LAUNCH_KEY_S_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectS, origin, 0.0f, GRAY);
    Vector2 startD = {LAUNCH_KEY_D_LINE_X, LAUNCH_KEY_D_LINE_Y_START};
    Vector2 endD = {LAUNCH_KEY_D_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startD, endD, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectD = {LAUNCH_KEY_D_X, LAUNCH_KEY_D_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectD, origin, 0.0f, GRAY);
    Vector2 startF = {LAUNCH_KEY_F_LINE_X, LAUNCH_KEY_F_LINE_Y_START};
    Vector2 endF = {LAUNCH_KEY_F_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startF, endF, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectF = {LAUNCH_KEY_F_X, LAUNCH_KEY_F_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectF, origin, 0.0f, GRAY);
    Vector2 startJ = {LAUNCH_KEY_J_LINE_X, LAUNCH_KEY_J_LINE_Y_START};
    Vector2 endJ = {LAUNCH_KEY_J_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startJ, endJ, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectJ = {LAUNCH_KEY_J_X, LAUNCH_KEY_J_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectJ, origin, 0.0f, GRAY);
    Vector2 startK = {LAUNCH_KEY_K_LINE_X, LAUNCH_KEY_K_LINE_Y_START};
    Vector2 endK = {LAUNCH_KEY_K_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startK, endK, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectK = {LAUNCH_KEY_K_X, LAUNCH_KEY_K_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectK, origin, 0.0f, GRAY);
    Vector2 startL = {LAUNCH_KEY_L_LINE_X, LAUNCH_KEY_L_LINE_Y_START};
    Vector2 endL = {LAUNCH_KEY_L_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startL, endL, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectL = {LAUNCH_KEY_L_X, LAUNCH_KEY_L_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectL, origin, 0.0f, GRAY);
    //Other keys
    Vector2 startEnter = {LAUNCH_KEY_ENTER_LINE_X, LAUNCH_KEY_ENTER_LINE_Y_START};
    Vector2 endEnter = {LAUNCH_KEY_ENTER_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startEnter, endEnter, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectEnter = {LAUNCH_KEY_ENTER_X, LAUNCH_KEY_ENTER_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectEnter, origin, 0.0f, GRAY);
    Vector2 startRShift = {LAUNCH_KEY_RSHIFT_LINE_X, LAUNCH_KEY_RSHIFT_LINE_Y_START};
    Vector2 endRShift = {LAUNCH_KEY_RSHIFT_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startRShift, endRShift, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectRShift = {LAUNCH_KEY_RSHIFT_X, LAUNCH_KEY_RSHIFT_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectRShift, origin, 0.0f, GRAY);
    Vector2 startLShift = {LAUNCH_KEY_LSHIFT_LINE_X, LAUNCH_KEY_LSHIFT_LINE_Y_START};
    Vector2 endLShift = {LAUNCH_KEY_LSHIFT_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startLShift, endLShift, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectLShift = {LAUNCH_KEY_LSHIFT_X, LAUNCH_KEY_LSHIFT_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectLShift, origin, 0.0f, GRAY);
    Vector2 startSpace = {LAUNCH_KEY_SPACE_LINE_X, LAUNCH_KEY_SPACE_LINE_Y_START};
    Vector2 endSpace = {LAUNCH_KEY_SPACE_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM2};
    DrawLineEx(startSpace, endSpace, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectSpace = {LAUNCH_KEY_SPACE_X, LAUNCH_KEY_SPACE_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectSpace, origin, 0.0f, GRAY);
    //Mouse
    Vector2 startMouse = {LAUNCH_KEY_MOUSE_LINE_X, LAUNCH_KEY_MOUSE_LINE_Y_START};
    Vector2 endMouse = {LAUNCH_KEY_MOUSE_LINE_X, LAUNCH_KEY_LINE_END_TOP1};
    DrawLineEx(startMouse, endMouse, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectMouse = {LAUNCH_KEY_MOUSE_X, LAUNCH_KEY_MOUSE_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectMouse, origin, 0.0f, GRAY);
    Vector2 startLClick = {LAUNCH_KEY_LCLICK_LINE_X, LAUNCH_KEY_LCLICK_LINE_Y_START};
    Vector2 endLClick = {LAUNCH_KEY_LCLICK_LINE_X, LAUNCH_KEY_LINE_END_BOTTOM1};
    DrawLineEx(startLClick, endLClick, LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rectLClick = {LAUNCH_KEY_LCLICK_X, LAUNCH_KEY_LCLICK_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectLClick, origin, 0.0f, GRAY);
    //Arrows
    Vector2 startArrows = {LAUNCH_KEY_ARROWS_LINE_X1, LAUNCH_KEY_ARROWS_LINE_Y};
    Vector2 endArrows = {LAUNCH_KEY_ARROWS_LINE_X2, LAUNCH_KEY_ARROWS_LINE_Y};
    DrawLineEx(startArrows, endArrows, LAUNCH_ARROW_THICKNESS, GRAY);
    Rectangle rectArrows = {LAUNCH_KEY_ARROWS_REC_X, LAUNCH_KEY_ARROWS_REC_Y, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rectArrows, origin, 0.0f, GRAY);
    //Set the text variables for the console and draw the non-shared ones
    if (strcmp(game->console, "Nintendo GameCube") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "A");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "X");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "Y");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "Z");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lt = {LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rt = {LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rs_Txt,    sizeof(rs_Txt),    "%s", "C");
        Vector2 rs_Size = MeasureTextEx(Var_GetFontBold(), rs_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rs = {LAUNCH_RS_TXT_X, LAUNCH_RS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rs_Txt, rs, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Nintendo DS") == 0) {
        snprintf(enter_Txt,  sizeof(enter_Txt),  "%s", "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(k_Txt,  sizeof(k_Txt),  "%s", "B");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(l_Txt,  sizeof(l_Txt),  "%s", "A");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 l = {LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(j_Txt,    sizeof(j_Txt),    "Y");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "X");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 i = {LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(q_Txt,    sizeof(q_Txt),    "L");
        Vector2 q_Size = MeasureTextEx(Var_GetFontBold(), q_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 q = {LAUNCH_KEY_Q_TXT_X, LAUNCH_KEY_Q_TXT_Y};
        DrawTextEx(Var_GetFontBold(), q_Txt, q, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(e_Txt,    sizeof(e_Txt),    "R");
        Vector2 e_Size = MeasureTextEx(Var_GetFontBold(), e_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 e = {LAUNCH_KEY_E_TXT_X, LAUNCH_KEY_E_TXT_Y};
        DrawTextEx(Var_GetFontBold(), e_Txt, e, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(space_Txt,    sizeof(space_Txt),    "LID");
        Vector2 space_Size = MeasureTextEx(Var_GetFontBold(), space_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 space = {LAUNCH_KEY_SPACE_TXT_X, LAUNCH_KEY_SPACE_TXT_Y};
        DrawTextEx(Var_GetFontBold(), space_Txt, space, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(tab_Txt,    sizeof(tab_Txt),    "MIC");
        Vector2 tab_Size = MeasureTextEx(Var_GetFontBold(), tab_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 tab = {LAUNCH_KEY_TAB_TXT_X, LAUNCH_KEY_TAB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), tab_Txt, tab, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(mouse_Txt,    sizeof(mouse_Txt),    "STYLUS");
        Vector2 mouse_Size = MeasureTextEx(Var_GetFontBold(), mouse_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 mouse = {LAUNCH_KEY_MOUSE_TXT_X, LAUNCH_KEY_MOUSE_TXT_Y};
        DrawTextEx(Var_GetFontBold(), mouse_Txt, mouse, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lClick_Txt,    sizeof(lClick_Txt),    "TOUCH");
        Vector2 lClick_Size = MeasureTextEx(Var_GetFontBold(), lClick_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lClick = {LAUNCH_KEY_LCLICK_TXT_X, LAUNCH_KEY_LCLICK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lClick_Txt, lClick, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sony PlayStation Portable") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(enter_Txt,  sizeof(enter_Txt),  "%s", "START");
        Vector2 enter_Size = MeasureTextEx(Var_GetFontBold(), enter_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 enter = {LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y};
        DrawTextEx(Var_GetFontBold(), enter_Txt, enter, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "X");
        snprintf(k_Txt,  sizeof(k_Txt),  "%s", "X");
        Vector2 k_Size = MeasureTextEx(Var_GetFontBold(), k_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 k = {LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y};
        DrawTextEx(Var_GetFontBold(), k_Txt, k, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "O");
        snprintf(l_Txt,  sizeof(l_Txt),  "%s", "O");
        Vector2 l_Size = MeasureTextEx(Var_GetFontBold(), l_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 l = {LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y};
        DrawTextEx(Var_GetFontBold(), l_Txt, l, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(a_Txt,    sizeof(a_Txt),    "D-PAD");
        Vector2 a_Size = MeasureTextEx(Var_GetFontBold(), a_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 a = {LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y};
        DrawTextEx(Var_GetFontBold(), a_Txt, a, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(s_Txt,    sizeof(s_Txt),    "D-PAD");
        Vector2 s_Size = MeasureTextEx(Var_GetFontBold(), s_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 s = {LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y};
        DrawTextEx(Var_GetFontBold(), s_Txt, s, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(d_Txt,    sizeof(d_Txt),    "D-PAD");
        Vector2 d_Size = MeasureTextEx(Var_GetFontBold(), d_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 d = {LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y};
        DrawTextEx(Var_GetFontBold(), d_Txt, d, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(w_Txt,    sizeof(w_Txt),    "D-PAD");
        Vector2 w_Size = MeasureTextEx(Var_GetFontBold(), w_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 w = {LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y};
        DrawTextEx(Var_GetFontBold(), w_Txt, w, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(back_Txt,  sizeof(back_Txt),  "%s", "SELECT");
        Vector2 back_Size = MeasureTextEx(Var_GetFontBold(), back_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 back = {LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y};
        DrawTextEx(Var_GetFontBold(), back_Txt, back, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rShift_Txt,    sizeof(rShift_Txt),    "SELECT");
        Vector2 rShift_Size = MeasureTextEx(Var_GetFontBold(), rShift_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rShift = {LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rShift_Txt, rShift, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "SQR");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(j_Txt,    sizeof(j_Txt),    "SQR");
        Vector2 j_Size = MeasureTextEx(Var_GetFontBold(), j_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 j = {LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y};
        DrawTextEx(Var_GetFontBold(), j_Txt, j, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "TRI");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(i_Txt,    sizeof(i_Txt),    "TRI");
        Vector2 i_Size = MeasureTextEx(Var_GetFontBold(), i_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 i = {LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y};
        DrawTextEx(Var_GetFontBold(), i_Txt, i, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "L");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(u_Txt,    sizeof(u_Txt),    "L");
        Vector2 u_Size = MeasureTextEx(Var_GetFontBold(), u_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 u = {LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y};
        DrawTextEx(Var_GetFontBold(), u_Txt, u, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "R");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(o_Txt,    sizeof(o_Txt),    "R");
        Vector2 o_Size = MeasureTextEx(Var_GetFontBold(), o_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 o = {LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y};
        DrawTextEx(Var_GetFontBold(), o_Txt, o, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(arrows_Txt,    sizeof(arrows_Txt),    "ANALOG");
        Vector2 arrows_Size = MeasureTextEx(Var_GetFontBold(), arrows_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 arrows = {LAUNCH_KEY_ARROWS_TXT_X, LAUNCH_KEY_ARROWS_TXT_Y};
        DrawTextEx(Var_GetFontBold(), arrows_Txt, arrows, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sega Saturn") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "B");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "C");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "D-PAD");
        snprintf(dPad_Txt,    sizeof(dPad_Txt),    "%s", "D-PAD");
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "A");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "Y");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "X");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "Z");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lt = {LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rt = {LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
    }
    else if (strcmp(game->console, "Sega Dreamcast") == 0) {
        snprintf(start_Txt, sizeof(start_Txt), "%s", "START");
        snprintf(rDown_Txt, sizeof(rDown_Txt), "%s", "A");
        snprintf(rRight_Txt,sizeof(rRight_Txt),"%s", "B");
        snprintf(ls_Txt,    sizeof(ls_Txt),    "%s", "STICK");
        snprintf(dPad_Txt,     sizeof(dPad_Txt),     "%s", "D-PAD");
        snprintf(rLeft_Txt, sizeof(rLeft_Txt), "%s", "X");
        Vector2 rLeft_Size = MeasureTextEx(Var_GetFontBold(), rLeft_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rLeft = {LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rLeft_Txt, rLeft, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rUp_Txt,   sizeof(rUp_Txt),   "%s", "Y");
        Vector2 rUp_Size = MeasureTextEx(Var_GetFontBold(), rUp_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rUp = {LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rUp_Txt, rUp, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lb_Txt,    sizeof(lb_Txt),    "%s", "Z");
        Vector2 lb_Size = MeasureTextEx(Var_GetFontBold(), lb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lb = {LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lb_Txt, lb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rb_Txt,    sizeof(rb_Txt),    "%s", "C");
        Vector2 rb_Size = MeasureTextEx(Var_GetFontBold(), rb_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rb = {LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rb_Txt, rb, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(lt_Txt,    sizeof(lt_Txt),    "%s", "L");
        Vector2 lt_Size = MeasureTextEx(Var_GetFontBold(), lt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 lt = {LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), lt_Txt, lt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
        snprintf(rt_Txt,    sizeof(rt_Txt),    "%s", "R");
        Vector2 rt_Size = MeasureTextEx(Var_GetFontBold(), rt_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
        Vector2 rt = {LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y};
        DrawTextEx(Var_GetFontBold(), rt_Txt, rt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
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
    Vector2 dPad_Size = MeasureTextEx(Var_GetFontBold(), dPad_Txt, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE);
    Vector2 dPad = {LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y};
    DrawTextEx(Var_GetFontBold(), dPad_Txt, dPad, LAUNCH_CONTROL_SIZE, LAUNCH_CONTROL_SPACE, BLACK);
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
    Vector2 launchTxt_Size = MeasureTextEx(Var_GetFontRegular(), LAUNCH_TXT, LAUNCH_SMALL_SIZE, LAUNCH_SPACE);
    Vector2 launchTxt_Pos = {LAUNCH_TITLE_X, LAUNCH_TITLE_Y};
    DrawTextEx(Var_GetFontRegular(), LAUNCH_TXT, launchTxt_Pos, LAUNCH_SMALL_SIZE, LAUNCH_SPACE, WHITE);
    //Name of game
    Vector2 gameTxt_Size = MeasureTextEx(Var_GetFontBold(), game->title, LAUNCH_TXT_SIZE, LAUNCH_SPACE);
    Vector2 gameTxt_Pos = {LAUNCH_GAME_X, LAUNCH_GAME_Y};
    DrawTextEx(Var_GetFontBold(), game->title, gameTxt_Pos, LAUNCH_TXT_SIZE, LAUNCH_SPACE, WHITE);
    char homeTxt[55];
    if (game->libRetro) {
        snprintf(homeTxt, sizeof(homeTxt), LAUNCH_HOME_LIB);
        UI_DrawControls_Lib(game);
        //sleep(5);
    }
    else {
        snprintf(homeTxt, sizeof(homeTxt), LAUNCH_HOME_EXT);
        UI_DrawControls_Ext(game);
    }
    //Home instructions
    Vector2 homeTxt_Size = MeasureTextEx(Var_GetFontRegular(), homeTxt, LAUNCH_SMALL_SIZE, LAUNCH_SPACE);
    Vector2 homeTxt_Pos = {LAUNCH_HOME_X, LAUNCH_HOME_Y};
    DrawTextEx(Var_GetFontRegular(), homeTxt, homeTxt_Pos, LAUNCH_SMALL_SIZE, LAUNCH_SPACE, WHITE);
    
}

//Display diagnostics on top left
void UI_DrawDispDiag(bool in_game) {
    //If diagnostics are being displayed
    if (Var_GetDiag()) {
        //Update the diagnostics
        Var_UpdateTemp();
        Var_UpdateClock();
        Var_UpdateEmuFps();
        Var_UpdateFrame();
        //Draw diagnostics
        Vector2 diagnosticsResolution = {DISP_DIAG_RES_X, DISP_DIAG_RES_Y};
        DrawTextEx(Var_GetFontRegular(), TextFormat("Resolution:  %d:%d", GetMonitorWidth(0), GetMonitorHeight(0)), diagnosticsResolution, DISP_DIAG_SIZE, 
            DISP_DIAG_SPACE, Var_GetColor3());
        Vector2 diagnosticsFPS = {DISP_DIAG_FPS_X, DISP_DIAG_FPS_Y};
        Vector2 temp = {DISP_DIAG_TEMP_X, DISP_DIAG_TEMP_Y};
        DrawTextEx(Var_GetFontRegular(), TextFormat("CPU Temp:  %.1f C", Var_GetTemp()), temp, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        Vector2 clock = {DISP_DIAG_CLOCK_X, DISP_DIAG_CLOCK_Y};
        DrawTextEx(Var_GetFontRegular(), TextFormat("Clock Speed:  %.2f GHz", Var_GetClock() / 1000000.0f), clock, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        if (in_game) {
            DrawTextEx(Var_GetFontRegular(), TextFormat("Emulator FPS:  %.1f / %.1f    Frame:  %.1f    Worst: %.1f", Var_GetEmuFps(), GetCoreTargetFPS(), Var_GetFrameAvg(), Var_GetFrameWorst()), diagnosticsFPS, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        }
        else {
            DrawTextEx(Var_GetFontRegular(), TextFormat("FPS:  %d    Frame:  %.1f    Worst: %.1f", GetFPS(), Var_GetFrameAvg(), Var_GetFrameWorst()), diagnosticsFPS, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
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
        if (IsKeyPressed(KEY_ESCAPE) || HOME_PRESS) {
            UIPause_Init();
            currentUIState = OPTIONS;
        }
        else if (IsKeyPressed(KEY_P) || START_PRESS) {
            Erase_Init();
            currentUIState = ERASE_DATA;
        }
        break;

    case SCROLL_GAMES:
        if (Var_GetScrollGames() == SCROLL_NO) {
            currentUIState = NORMAL;
            alphaSelectTxt_Blink = true;
            alphaSelectTxt_TimeElapsed = 0.0f;
        }
        if (IsKeyPressed(KEY_ESCAPE) || HOME_PRESS) {
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
        if (IsKeyPressed(KEY_ESCAPE) || HOME_PRESS) {
            UIPause_Init();
            currentUIState = OPTIONS;
        }
        break;

    case OPTIONS:
        if (HOME_PRESS || ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Var_GetDisplayBright() && !Var_GetDisplayTheme())) {
            currentUIState = NORMAL;
            Controller_SetWasPressed_B(true);
        }
        break;

    case ERASE_DATA:
        
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
        if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
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

    case ERASE_DATA:
        UI_DrawBase();
        UI_ChangeAlpha_Static();
        UI_DrawCateg_Static();
        UI_DrawGames_Normal();
        //Draw bumpers
        UI_DrawTop();
        //Draw bottom section of screen
        UI_DrawBottom();
        UI_DrawDispDiag(false);
        Erase_Tick(Games_GetDisplayed(3), &currentUIState);
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