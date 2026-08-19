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
#include "controller.h"
#include "uipause.h"
#include "brightness.h"
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
//Image of controller
static Texture2D controlsImg;
static Texture2D keyImg;
static Texture2D mouseImg;
static Texture2D arrowsImg;
static Texture2D xbox;

//Whether the text is blinking on or not
static bool alphaSelectTxt_Blink = true;


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
static void UI_ResetCoords_Games(void) {
    // Reset x coordinates
    Games_GetDisplayed(GAME_LEFT3)->x = LEFT3_GAME_X;
    Games_GetDisplayed(GAME_LEFT2)->x = LEFT2_GAME_X;
    Games_GetDisplayed(GAME_LEFT1)->x = LEFT1_GAME_X;
    Games_GetDisplayed(CURRENT_GAME)->x = CENTER_GAME_X;
    Games_GetDisplayed(GAME_RIGHT1)->x = RIGHT1_GAME_X;
    Games_GetDisplayed(GAME_RIGHT2)->x = RIGHT2_GAME_X;
    Games_GetDisplayed(GAME_RIGHT3)->x = RIGHT3_GAME_X;
    // Reset y coordinates
    Games_GetDisplayed(GAME_LEFT3)->y = SIDE3_GAME_Y;
    Games_GetDisplayed(GAME_LEFT2)->y = SIDE2_GAME_Y;
    Games_GetDisplayed(GAME_LEFT1)->y = SIDE1_GAME_Y;
    Games_GetDisplayed(CURRENT_GAME)->y = CENTER_GAME_Y;
    Games_GetDisplayed(GAME_RIGHT1)->y = SIDE1_GAME_Y;
    Games_GetDisplayed(GAME_RIGHT2)->y = SIDE2_GAME_Y;
    Games_GetDisplayed(GAME_RIGHT3)->y = SIDE3_GAME_Y;
    // Reset width values
    Games_GetDisplayed(GAME_LEFT3)->w = SIDE3_GAME_W;
    Games_GetDisplayed(GAME_LEFT2)->w = SIDE2_GAME_W;
    Games_GetDisplayed(GAME_LEFT1)->w = SIDE1_GAME_W;
    Games_GetDisplayed(CURRENT_GAME)->w = CENTER_GAME_W;
    Games_GetDisplayed(GAME_RIGHT1)->w = SIDE1_GAME_W;
    Games_GetDisplayed(GAME_RIGHT2)->w = SIDE2_GAME_W;
    Games_GetDisplayed(GAME_RIGHT3)->w = SIDE3_GAME_W;
    // Reset height values
    Games_GetDisplayed(GAME_LEFT3)->h = SIDE3_GAME_H;
    Games_GetDisplayed(GAME_LEFT2)->h = SIDE2_GAME_H;
    Games_GetDisplayed(GAME_LEFT1)->h = SIDE1_GAME_H;
    Games_GetDisplayed(CURRENT_GAME)->h = CENTER_GAME_H;
    Games_GetDisplayed(GAME_RIGHT1)->h = SIDE1_GAME_H;
    Games_GetDisplayed(GAME_RIGHT2)->h = SIDE2_GAME_H;
    Games_GetDisplayed(GAME_RIGHT3)->h = SIDE3_GAME_H;
}

//Reset the coordinates for the displayed games after category scroll
static void UI_ResetCoords_Categ(void) {
    //If scrolling to the right
    if (Var_GetScrollCateg() == SCROLL_RIGHT) {
        //Reset x coordinates
        Games_GetNew(NEW_GAME_LEFT2)->x = LEFT2_SCROLLL_X;
        Games_GetNew(NEW_GAME_LEFT1)->x = LEFT1_SCROLLL_X;
        Games_GetNew(NEW_GAME_CENTER)->x = CENTER_SCROLLL_X;
        Games_GetNew(NEW_GAME_RIGHT1)->x = RIGHT1_SCROLLL_X;
        Games_GetNew(NEW_GAME_RIGHT2)->x = RIGHT2_SCROLLL_X;
    }
    //If scrolling to the left
    else {
        //Reset x coordinates
        Games_GetNew(NEW_GAME_LEFT2)->x = LEFT2_SCROLLR_X;
        Games_GetNew(NEW_GAME_LEFT1)->x = LEFT1_SCROLLR_X;
        Games_GetNew(NEW_GAME_CENTER)->x = CENTER_SCROLLR_X;
        Games_GetNew(NEW_GAME_RIGHT1)->x = RIGHT1_SCROLLR_X;
        Games_GetNew(NEW_GAME_RIGHT2)->x = RIGHT2_SCROLLR_X;
    }
    //Reset y coordinates
    Games_GetNew(NEW_GAME_LEFT2)->y = SIDE2_GAME_Y;
    Games_GetNew(NEW_GAME_LEFT1)->y = SIDE1_GAME_Y;
    Games_GetNew(NEW_GAME_CENTER)->y = CENTER_GAME_Y;
    Games_GetNew(NEW_GAME_RIGHT1)->y = SIDE1_GAME_Y;
    Games_GetNew(NEW_GAME_RIGHT2)->y = SIDE2_GAME_Y;
    //Reset width values
    Games_GetNew(NEW_GAME_LEFT2)->w = SIDE2_GAME_W;
    Games_GetNew(NEW_GAME_LEFT1)->w = SIDE1_GAME_W;
    Games_GetNew(NEW_GAME_CENTER)->w = CENTER_GAME_W;
    Games_GetNew(NEW_GAME_RIGHT1)->w = SIDE1_GAME_W;
    Games_GetNew(NEW_GAME_RIGHT2)->w = SIDE2_GAME_W;
    //Reset height values
    Games_GetNew(NEW_GAME_LEFT2)->h = SIDE2_GAME_H;
    Games_GetNew(NEW_GAME_LEFT1)->h = SIDE1_GAME_H;
    Games_GetNew(NEW_GAME_CENTER)->h = CENTER_GAME_H;
    Games_GetNew(NEW_GAME_RIGHT1)->h = SIDE1_GAME_H;
    Games_GetNew(NEW_GAME_RIGHT2)->h = SIDE2_GAME_H;
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
void UI_DrawOptions(void) {
    // Draw options button
    DrawCircle(XBOX_X, XBOX_Y, XBOX_RADIUS, WHITE);
    Rectangle sourceRect = {0.0f, 0.0f, (float)xbox.width, (float)xbox.height};
    Rectangle destRect = {UI_CenterImg_X(XBOX_SIZE, XBOX_X), UI_CenterImg_Y(XBOX_SIZE, XBOX_Y), XBOX_SIZE, XBOX_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(xbox, sourceRect, destRect, origin, 0.0f, WHITE);
    //Draw options text
    Vector2 optionsSize = MeasureTextEx(Var_GetFontBold(), OPTIONS_TXT, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE);
    Vector2 options = {OPTIONS_TXT_X, OPTIONS_TXT_Y};
    DrawTextEx(Var_GetFontRegular(), OPTIONS_TXT, options, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE, Var_GetColor3());
}

//Function to draw the bumpers
static void UI_DrawBumpers(void) {
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
static void UI_DrawTop(void) {
    UI_DrawBumpers();
    UI_DrawOptions();
    //Draw small spider logo
    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(SMALL_LOGO_SIZE, CENTER_X), UI_CenterImg_Y(SMALL_LOGO_SIZE, SMALL_LOGO_Y), SMALL_LOGO_SIZE, SMALL_LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Draw the select controls at the bottom
static void UI_DrawBottom(void) {
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
    Vector2 gameSize = MeasureTextEx(Var_GetFontBold(), Games_GetDisplayed(CURRENT_GAME)->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
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
    DrawTextEx(Var_GetFontBold(), Games_GetDisplayed(CURRENT_GAME)->title, game, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    DrawTextEx(Var_GetFontBold(), Games_GetDisplayed(CURRENT_GAME)->console, console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
    DrawTextEx(Var_GetFontBold(), BOTTOM_TXT, bottom, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), Var_GetAlphaSelect()));
}

//Load logos
void UI_LoadLogos(void) {
    spiderLogo = LoadTexture("assets/images/other/logo.png");
    xbox = LoadTexture("assets/images/other/xbox.png");
}

//Draw the boot screen
void UI_DrawBootScreen(void) {
    ClearBackground(BACKGROUND_CLR);
    //Draw logo
    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(LOGO_SIZE, CENTER_X), UI_CenterImg_Y(LOGO_SIZE, Var_GetMonitorHeight() / 2), LOGO_SIZE, LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Load images of controller/keybaord and mouse
void UI_LoadControlsImgs(void) {
    controlsImg = LoadTexture("assets/images/other/controller.png");
    keyImg = LoadTexture("assets/images/other/keyboard.png");
    mouseImg = LoadTexture("assets/images/other/mouse.png");
    arrowsImg = LoadTexture("assets/images/other/arrow_keys.png");
}

//Draw specific control with normal size
static void UI_DrawControlTxt(const char* control, float x, float y, float font_size) {
    char txt[LAUNCH_STR_LEN];
    snprintf(txt, sizeof(txt), "%s", control);
    Vector2 size = MeasureTextEx(Var_GetFontBold(), txt, font_size, LAUNCH_CONTROL_SPACE);
    Vector2 position = {x - size.x / 2.0f, y - size.y / 2.0f};
    DrawTextEx(Var_GetFontBold(), txt, position, font_size, LAUNCH_CONTROL_SPACE, BLACK);
}

//Draw box and line for the keyboard and mouse controls
static void UI_DrawKeyBox(float lineStartX, float lineEndX, float lineStartY, float lineEndY, float boxX, float boxY) {
    Vector2 origin = {0.0f, 0.0f};
    Vector2 start = {lineStartX, lineStartY};
    Vector2 end = {lineEndX, lineEndY};
    DrawLineEx(start, end,LAUNCH_KEY_THICKNESS, GRAY);
    Rectangle rect = {boxX, boxY, LAUNCH_KEY_REC_W, LAUNCH_KEY_REC_H};
    DrawRectanglePro(rect, origin, 0.0f, GRAY);
}

//Draw the controls for the game
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
    UI_DrawKeyBox(LAUNCH_KEY_TAB_LINE_X, LAUNCH_KEY_TAB_LINE_X, LAUNCH_KEY_TAB_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                        LAUNCH_KEY_TAB_X, LAUNCH_KEY_TAB_Y);
    UI_DrawKeyBox(LAUNCH_KEY_Q_LINE_X, LAUNCH_KEY_Q_LINE_X, LAUNCH_KEY_Q_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP2,
                        LAUNCH_KEY_Q_X, LAUNCH_KEY_Q_Y);
    UI_DrawKeyBox(LAUNCH_KEY_W_LINE_X, LAUNCH_KEY_W_LINE_X, LAUNCH_KEY_W_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                        LAUNCH_KEY_W_X, LAUNCH_KEY_W_Y);
    UI_DrawKeyBox(LAUNCH_KEY_E_LINE_X, LAUNCH_KEY_E_LINE_X, LAUNCH_KEY_E_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP2,
                        LAUNCH_KEY_E_X, LAUNCH_KEY_E_Y);
    UI_DrawKeyBox(LAUNCH_KEY_U_LINE_X, LAUNCH_KEY_U_LINE_X, LAUNCH_KEY_U_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                        LAUNCH_KEY_U_X, LAUNCH_KEY_U_Y);
    UI_DrawKeyBox(LAUNCH_KEY_I_LINE_X, LAUNCH_KEY_I_LINE_X, LAUNCH_KEY_I_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP2,
                        LAUNCH_KEY_I_X, LAUNCH_KEY_I_Y);
    UI_DrawKeyBox(LAUNCH_KEY_O_LINE_X, LAUNCH_KEY_O_LINE_X, LAUNCH_KEY_O_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                        LAUNCH_KEY_O_X, LAUNCH_KEY_O_Y);
    //2nd keyboard row
    UI_DrawKeyBox(LAUNCH_KEY_A_LINE_X, LAUNCH_KEY_A_LINE_X, LAUNCH_KEY_A_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                        LAUNCH_KEY_A_X, LAUNCH_KEY_A_Y);
    UI_DrawKeyBox(LAUNCH_KEY_S_LINE_X, LAUNCH_KEY_S_LINE_X, LAUNCH_KEY_S_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                        LAUNCH_KEY_S_X, LAUNCH_KEY_S_Y);
    UI_DrawKeyBox(LAUNCH_KEY_D_LINE_X, LAUNCH_KEY_D_LINE_X, LAUNCH_KEY_D_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                        LAUNCH_KEY_D_X, LAUNCH_KEY_D_Y);
    UI_DrawKeyBox(LAUNCH_KEY_F_LINE_X, LAUNCH_KEY_F_LINE_X, LAUNCH_KEY_F_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                        LAUNCH_KEY_F_X, LAUNCH_KEY_F_Y);
    UI_DrawKeyBox(LAUNCH_KEY_J_LINE_X, LAUNCH_KEY_J_LINE_X, LAUNCH_KEY_J_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                        LAUNCH_KEY_J_X, LAUNCH_KEY_J_Y);
    UI_DrawKeyBox(LAUNCH_KEY_K_LINE_X, LAUNCH_KEY_K_LINE_X, LAUNCH_KEY_K_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                        LAUNCH_KEY_K_X, LAUNCH_KEY_K_Y);
    UI_DrawKeyBox(LAUNCH_KEY_L_LINE_X, LAUNCH_KEY_L_LINE_X, LAUNCH_KEY_L_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                        LAUNCH_KEY_L_X, LAUNCH_KEY_L_Y);
    //Other keys
    UI_DrawKeyBox(LAUNCH_KEY_ENTER_LINE_X, LAUNCH_KEY_ENTER_LINE_X, LAUNCH_KEY_ENTER_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                        LAUNCH_KEY_ENTER_X, LAUNCH_KEY_ENTER_Y);
    UI_DrawKeyBox(LAUNCH_KEY_RSHIFT_LINE_X, LAUNCH_KEY_RSHIFT_LINE_X, LAUNCH_KEY_RSHIFT_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                        LAUNCH_KEY_RSHIFT_X, LAUNCH_KEY_RSHIFT_Y);
    UI_DrawKeyBox(LAUNCH_KEY_LSHIFT_LINE_X, LAUNCH_KEY_LSHIFT_LINE_X, LAUNCH_KEY_LSHIFT_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                        LAUNCH_KEY_LSHIFT_X, LAUNCH_KEY_LSHIFT_Y);
    UI_DrawKeyBox(LAUNCH_KEY_SPACE_LINE_X, LAUNCH_KEY_SPACE_LINE_X, LAUNCH_KEY_SPACE_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                        LAUNCH_KEY_SPACE_X, LAUNCH_KEY_SPACE_Y);
    //Mouse
    UI_DrawKeyBox(LAUNCH_KEY_MOUSE_LINE_X, LAUNCH_KEY_MOUSE_LINE_X, LAUNCH_KEY_MOUSE_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                        LAUNCH_KEY_MOUSE_X, LAUNCH_KEY_MOUSE_Y);
    UI_DrawKeyBox(LAUNCH_KEY_LCLICK_LINE_X, LAUNCH_KEY_LCLICK_LINE_X, LAUNCH_KEY_LCLICK_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                        LAUNCH_KEY_LCLICK_X, LAUNCH_KEY_LCLICK_Y);
    //Arrows
    UI_DrawKeyBox(LAUNCH_KEY_ARROWS_LINE_X1, LAUNCH_KEY_ARROWS_LINE_X2, LAUNCH_KEY_ARROWS_LINE_Y, LAUNCH_KEY_ARROWS_LINE_Y,
                        LAUNCH_KEY_ARROWS_REC_X, LAUNCH_KEY_ARROWS_REC_Y);
    //Common home control for all consoles
    UI_DrawControlTxt("PAUSE", LAUNCH_HOME_TXT_X, LAUNCH_HOME_TXT_Y, LAUNCH_CONTROL_SIZE);
    //Draw the controls depending on the console
    if (strcmp(game->console, "Sega Genesis") == 0 || strcmp(game->console, "Sega CD") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("MODE", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("MODE", LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Y", LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Y", LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Z", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Z", LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Sega Game Gear") == 0 || strcmp(game->console, "Sega Master System") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("1", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("1", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("2", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("2", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "TurboGrafx-16") == 0 || strcmp(game->console, "TurboGrafx-CD") == 0) {
        UI_DrawControlTxt("RUN", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("RUN", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("I", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("I", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("II", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("II", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Neo Geo Pocket Color") == 0) {
        UI_DrawControlTxt("OPTION", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("OPTION", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Nintendo Entertainment System") == 0
             || strcmp(game->console, "Game Boy Color") == 0
             || strcmp(game->console, "Atari Lynx") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Super Nintendo Entertainment System") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Y", LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Y", LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Game Boy Advance") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Sony PlayStation") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("O", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("O", LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SQR", LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SQR", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("TRI", LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("TRI", LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L1", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L1", LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R1", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R1", LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L2", LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L2", LAUNCH_KEY_Q_TXT_X, LAUNCH_KEY_Q_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R2", LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R2", LAUNCH_KEY_E_TXT_X, LAUNCH_KEY_E_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Nintendo 64") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_ARROWS_TXT_X, LAUNCH_KEY_ARROWS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_KEY_Q_TXT_X, LAUNCH_KEY_Q_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_KEY_E_TXT_X, LAUNCH_KEY_E_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Z", LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Z", LAUNCH_KEY_SPACE_TXT_X, LAUNCH_KEY_SPACE_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_RS_TXT_X, LAUNCH_RS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Arcade") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("1", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("1", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("2", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("2", LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("COIN", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("COIN", LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("3", LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("3", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("4", LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("4", LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("5", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("5", LAUNCH_KEY_U_TXT_X, LAUNCH_KEY_U_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("6", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("6", LAUNCH_KEY_O_TXT_X, LAUNCH_KEY_O_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "PC") == 0) {
        UI_DrawControlTxt("ENTER", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("ENTER", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("USE", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("USE", LAUNCH_KEY_SPACE_TXT_X, LAUNCH_KEY_SPACE_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("MOVE/RUN", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM1);
        UI_DrawControlTxt("STRAFE", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM1);
        UI_DrawControlTxt("MOVE", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM1);
        UI_DrawControlTxt("STRAFE", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM1);
        UI_DrawControlTxt("MOVE", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM1);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("RUN", LAUNCH_KEY_LSHIFT_TXT_X, LAUNCH_KEY_LSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("MAP", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("MAP", LAUNCH_KEY_TAB_TXT_X, LAUNCH_KEY_TAB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("FIRE", LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("FIRE", LAUNCH_KEY_LCLICK_TXT_X, LAUNCH_KEY_LCLICK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("FIRE", LAUNCH_KEY_F_TXT_X, LAUNCH_KEY_F_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SWITCH GUNS", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM2);
        UI_DrawControlTxt("SWITCH GUNS", LAUNCH_KEY_Q_TXT_X, LAUNCH_KEY_Q_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM2);
        UI_DrawControlTxt("SWITCH GUNS", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM2);
        UI_DrawControlTxt("SWITCH GUNS", LAUNCH_KEY_E_TXT_X, LAUNCH_KEY_E_TXT_Y, LAUNCH_CONTROL_SIZE_DOOM2);
        UI_DrawControlTxt("TURN", LAUNCH_RS_TXT_X, LAUNCH_RS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("TURN", LAUNCH_KEY_MOUSE_TXT_X, LAUNCH_KEY_MOUSE_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
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
    UI_DrawKeyBox(LAUNCH_KEY_TAB_LINE_X, LAUNCH_KEY_TAB_LINE_X, LAUNCH_KEY_TAB_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                LAUNCH_KEY_TAB_X, LAUNCH_KEY_TAB_Y);
    UI_DrawKeyBox(LAUNCH_KEY_Q_LINE_X, LAUNCH_KEY_Q_LINE_X, LAUNCH_KEY_Q_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP2,
                LAUNCH_KEY_Q_X, LAUNCH_KEY_Q_Y);
    UI_DrawKeyBox(LAUNCH_KEY_W_LINE_X, LAUNCH_KEY_W_LINE_X, LAUNCH_KEY_W_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                LAUNCH_KEY_W_X, LAUNCH_KEY_W_Y);
    UI_DrawKeyBox(LAUNCH_KEY_E_LINE_X, LAUNCH_KEY_E_LINE_X, LAUNCH_KEY_E_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP2,
                LAUNCH_KEY_E_X, LAUNCH_KEY_E_Y);
    UI_DrawKeyBox(LAUNCH_KEY_U_LINE_X, LAUNCH_KEY_U_LINE_X, LAUNCH_KEY_U_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                LAUNCH_KEY_U_X, LAUNCH_KEY_U_Y);
    UI_DrawKeyBox(LAUNCH_KEY_I_LINE_X, LAUNCH_KEY_I_LINE_X, LAUNCH_KEY_I_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP2,
                LAUNCH_KEY_I_X, LAUNCH_KEY_I_Y);
    UI_DrawKeyBox(LAUNCH_KEY_O_LINE_X, LAUNCH_KEY_O_LINE_X, LAUNCH_KEY_O_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                LAUNCH_KEY_O_X, LAUNCH_KEY_O_Y);

    //2nd keyboard row
    UI_DrawKeyBox(LAUNCH_KEY_A_LINE_X, LAUNCH_KEY_A_LINE_X, LAUNCH_KEY_A_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                LAUNCH_KEY_A_X, LAUNCH_KEY_A_Y);
    UI_DrawKeyBox(LAUNCH_KEY_S_LINE_X, LAUNCH_KEY_S_LINE_X, LAUNCH_KEY_S_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                LAUNCH_KEY_S_X, LAUNCH_KEY_S_Y);
    UI_DrawKeyBox(LAUNCH_KEY_D_LINE_X, LAUNCH_KEY_D_LINE_X, LAUNCH_KEY_D_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                LAUNCH_KEY_D_X, LAUNCH_KEY_D_Y);
    UI_DrawKeyBox(LAUNCH_KEY_F_LINE_X, LAUNCH_KEY_F_LINE_X, LAUNCH_KEY_F_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                LAUNCH_KEY_F_X, LAUNCH_KEY_F_Y);
    UI_DrawKeyBox(LAUNCH_KEY_J_LINE_X, LAUNCH_KEY_J_LINE_X, LAUNCH_KEY_J_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                LAUNCH_KEY_J_X, LAUNCH_KEY_J_Y);
    UI_DrawKeyBox(LAUNCH_KEY_K_LINE_X, LAUNCH_KEY_K_LINE_X, LAUNCH_KEY_K_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                LAUNCH_KEY_K_X, LAUNCH_KEY_K_Y);
    UI_DrawKeyBox(LAUNCH_KEY_L_LINE_X, LAUNCH_KEY_L_LINE_X, LAUNCH_KEY_L_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                LAUNCH_KEY_L_X, LAUNCH_KEY_L_Y);

    //Other keys
    UI_DrawKeyBox(LAUNCH_KEY_ENTER_LINE_X, LAUNCH_KEY_ENTER_LINE_X, LAUNCH_KEY_ENTER_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                LAUNCH_KEY_ENTER_X, LAUNCH_KEY_ENTER_Y);
    UI_DrawKeyBox(LAUNCH_KEY_RSHIFT_LINE_X, LAUNCH_KEY_RSHIFT_LINE_X, LAUNCH_KEY_RSHIFT_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                LAUNCH_KEY_RSHIFT_X, LAUNCH_KEY_RSHIFT_Y);
    UI_DrawKeyBox(LAUNCH_KEY_LSHIFT_LINE_X, LAUNCH_KEY_LSHIFT_LINE_X, LAUNCH_KEY_LSHIFT_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                LAUNCH_KEY_LSHIFT_X, LAUNCH_KEY_LSHIFT_Y);
    UI_DrawKeyBox(LAUNCH_KEY_SPACE_LINE_X, LAUNCH_KEY_SPACE_LINE_X, LAUNCH_KEY_SPACE_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM2,
                LAUNCH_KEY_SPACE_X, LAUNCH_KEY_SPACE_Y);

    //Mouse
    UI_DrawKeyBox(LAUNCH_KEY_MOUSE_LINE_X, LAUNCH_KEY_MOUSE_LINE_X, LAUNCH_KEY_MOUSE_LINE_Y_START, LAUNCH_KEY_LINE_END_TOP1,
                LAUNCH_KEY_MOUSE_X, LAUNCH_KEY_MOUSE_Y);
    UI_DrawKeyBox(LAUNCH_KEY_LCLICK_LINE_X, LAUNCH_KEY_LCLICK_LINE_X, LAUNCH_KEY_LCLICK_LINE_Y_START, LAUNCH_KEY_LINE_END_BOTTOM1,
                LAUNCH_KEY_LCLICK_X, LAUNCH_KEY_LCLICK_Y);

    //Arrows
    UI_DrawKeyBox(LAUNCH_KEY_ARROWS_LINE_X1, LAUNCH_KEY_ARROWS_LINE_X2, LAUNCH_KEY_ARROWS_LINE_Y, LAUNCH_KEY_ARROWS_LINE_Y,
                LAUNCH_KEY_ARROWS_REC_X, LAUNCH_KEY_ARROWS_REC_Y);
    //Draw the controls depending on the console
    if (strcmp(game->console, "Nintendo GameCube") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Y", LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Z", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_RS_TXT_X, LAUNCH_RS_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Nintendo DS") == 0) {
        UI_DrawControlTxt("START", LAUNCH_KEY_ENTER_TXT_X, LAUNCH_KEY_ENTER_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_KEY_RSHIFT_TXT_X, LAUNCH_KEY_RSHIFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_A_TXT_X, LAUNCH_KEY_A_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_S_TXT_X, LAUNCH_KEY_S_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_D_TXT_X, LAUNCH_KEY_D_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_KEY_W_TXT_X, LAUNCH_KEY_W_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_KEY_K_TXT_X, LAUNCH_KEY_K_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_KEY_L_TXT_X, LAUNCH_KEY_L_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Y", LAUNCH_KEY_J_TXT_X, LAUNCH_KEY_J_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_KEY_I_TXT_X, LAUNCH_KEY_I_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_KEY_Q_TXT_X, LAUNCH_KEY_Q_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_KEY_E_TXT_X, LAUNCH_KEY_E_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("LID", LAUNCH_KEY_SPACE_TXT_X, LAUNCH_KEY_SPACE_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("MIC", LAUNCH_KEY_TAB_TXT_X, LAUNCH_KEY_TAB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STYLUS", LAUNCH_KEY_MOUSE_TXT_X, LAUNCH_KEY_MOUSE_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("TOUCH", LAUNCH_KEY_LCLICK_TXT_X, LAUNCH_KEY_LCLICK_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Sony PlayStation Portable") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("O", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SELECT", LAUNCH_BACK_TXT_X, LAUNCH_BACK_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("SQR", LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("TRI", LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Sega Saturn") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Y", LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Z", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y, LAUNCH_CONTROL_SIZE);
    }
    else if (strcmp(game->console, "Sega Dreamcast") == 0) {
        UI_DrawControlTxt("START", LAUNCH_START_TXT_X, LAUNCH_START_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("A", LAUNCH_RIGHT_FACE_DOWN_TXT_X, LAUNCH_RIGHT_FACE_DOWN_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("B", LAUNCH_RIGHT_FACE_RIGHT_TXT_X, LAUNCH_RIGHT_FACE_RIGHT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("STICK", LAUNCH_LS_TXT_X, LAUNCH_LS_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("D-PAD", LAUNCH_DPAD_TXT_X, LAUNCH_DPAD_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("X", LAUNCH_RIGHT_FACE_LEFT_TXT_X, LAUNCH_RIGHT_FACE_LEFT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Y", LAUNCH_RIGHT_FACE_UP_TXT_X, LAUNCH_RIGHT_FACE_UP_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("Z", LAUNCH_LB_TXT_X, LAUNCH_LB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("C", LAUNCH_RB_TXT_X, LAUNCH_RB_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("L", LAUNCH_LT_TXT_X, LAUNCH_LT_TXT_Y, LAUNCH_CONTROL_SIZE);
        UI_DrawControlTxt("R", LAUNCH_RT_TXT_X, LAUNCH_RT_TXT_Y, LAUNCH_CONTROL_SIZE);
    }//Draw outline
    Rectangle outline = {LAUNCH_CONTROLLER_X, LAUNCH_CONTROLLER_Y, LAUNCH_CONTROLLER_W * 2, LAUNCH_CONTROLLER_H};
    DrawRectangleLinesEx(outline, LAUNCH_CONTROL_OUTLINE_THICKNESS, GRAY);
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
    //Draw the controls based on whether it is libretro or external app
    if (game->libRetro) {
        snprintf(homeTxt, sizeof(homeTxt), LAUNCH_HOME_LIB);
        UI_DrawControls_Lib(game);
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
        //Change the FPS depending on whether in game or not
        if (in_game) {
            DrawTextEx(Var_GetFontRegular(), TextFormat("Emulator FPS:  %.1f / %.1f    Frame:  %.1f    Worst: %.1f", Var_GetEmuFps(), GetCoreTargetFPS(), Var_GetFrameAvg(), Var_GetFrameWorst()), diagnosticsFPS, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        }
        else {
            DrawTextEx(Var_GetFontRegular(), TextFormat("FPS:  %d    Frame:  %.1f    Worst: %.1f", GetFPS(), Var_GetFrameAvg(), Var_GetFrameWorst()), diagnosticsFPS, DISP_DIAG_SIZE, DISP_DIAG_SPACE, Var_GetColor3());
        }
    }
}

//Draw base of the main screen
static void UI_DrawBase(void) {
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
void UI_ChangeAlpha_Static(void) {
    //Blink the correct objects
    if (alphaSelectTxt_Blink == true) {
        // If less than 1, increment
        if (Var_GetAlphaSelect() < 1.0f) {
            Var_AddAlphaSelect(ALPHA_SELECT_INCREMENT_STATIC * GetFrameTime() * FPS);
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
    else {
        // If less than 1, increment
        if (Var_GetAlphaSelect() > ALPHA_SELECT_MIN) {
            Var_AddAlphaSelect(- ALPHA_SELECT_INCREMENT_STATIC * GetFrameTime() * FPS);
        }
        //If alpha has gotten low enough
        else if (Var_GetAlphaSelect() <= ALPHA_SELECT_MIN) {
            alphaSelectTxt_TimeElapsed += GetFrameTime();
            //If enough time has passed, keep blinking
            if (alphaSelectTxt_TimeElapsed >= SELECT_BLINK_TIME_LOW) {
                alphaSelectTxt_Blink = true;
                alphaSelectTxt_TimeElapsed = 0.0f;
            }
        }
    }
    //If less than 1, increment
    if (alphaGames < 1.0f) {
        alphaGames += (ALPHA_GAMES_INCREMENT * GetFrameTime() * FPS);
    }
    //If greater than 1, bring back to 1
    else if (alphaGames > 1.0f) {
        alphaGames = 1.0f;
    }
    //If less than 1, increment
    if (alphaSelectBox < 1.0f) {
        alphaSelectBox += (ALPHA_SELECT_BOX_INCREMENT * GetFrameTime() * FPS);
    }
    //If greater than 1, bring back to 1
    else if (alphaSelectBox > 1.0f) {
        alphaSelectBox = 1.0f;
    }
}

//Change alphas when scrolling grames
static void UI_ChangeAlpha_ScrollGames(void) {
    //If less than 0, bring to 0
    if (Var_GetAlphaSelect() < 0.0f) {
        Var_SetAlphaSelect(0.0f);
    }
    //If greater than 0, decrement
    else if (Var_GetAlphaSelect() > 0.0f) {
        Var_AddAlphaSelect(- ALPHA_SELECT_INCREMENT_SCROLL * GetFrameTime() * FPS);
    }
    //If less than 0, bring to 0
    if (alphaGames < 0.0f) {
        alphaGames = 0.0f;
    }
    //If greater than 0, decrement
    else if (alphaGames > 0.0f) {
        alphaGames -= (ALPHA_GAMES_INCREMENT * GetFrameTime() * FPS);
    }
    //If less than 1, increment
    if (alphaSelectBox < 1.0f) {
        alphaSelectBox += (ALPHA_SELECT_BOX_INCREMENT * GetFrameTime() * FPS);
    }
    //If greater than 1, bring to 1
    else if (alphaSelectBox > 1.0f) {
        alphaSelectBox = 1.0f;
    }
}

//Change alphas when scrolling categories
static void UI_ChangeAlpha_ScrollCateg(void) {
    // If less than 0, bring to 0
    if (Var_GetAlphaSelect() < 0.0f) {
        Var_SetAlphaSelect(0.0f);
    }
    // If greater than 0, decrement
    else if (Var_GetAlphaSelect() > 0.0f) {
        Var_AddAlphaSelect(- ALPHA_SELECT_INCREMENT_SCROLL * GetFrameTime() * FPS);
    }
    // If less than 0, bring to 0
    if (alphaGames < 0.0f) {
        alphaGames = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaGames > 0.0f) {
        alphaGames -= (ALPHA_GAMES_INCREMENT * GetFrameTime() * FPS);
    }
    if (alphaSelectBox > 0.0f) {
        alphaSelectBox -= (ALPHA_SELECT_BOX_INCREMENT * GetFrameTime() * FPS);
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
        alphaCategories_In += (ALPHA_CATEG_IN_INCREMENT * GetFrameTime() * FPS);
    }
    // If less than 0, bring to 0
    if (alphaCategories_Out < 0.0f) {
        alphaCategories_Out = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaCategories_Out > 0.0f) {
        alphaCategories_Out -= (ALPHA_CATEG_OUT_INCREMENT * GetFrameTime() * FPS);
    }
}

//Draw the cateogires when they are static
static void UI_DrawCateg_Static(void) {
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
static void UI_DrawCateg_Scroll(void) {
    //If scrolling to the right
    if (Var_GetScrollCateg() == SCROLL_RIGHT) {
        //If images close enough to final spot
        if (fabsf(Games_GetNew(NEW_GAME_CENTER)->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD) {
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
        Games_GetNew(NEW_GAME_LEFT2)->x = Lerp(Games_GetNew(NEW_GAME_LEFT2)->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(NEW_GAME_LEFT1)->x = Lerp(Games_GetNew(NEW_GAME_LEFT1)->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(NEW_GAME_CENTER)->x = Lerp(Games_GetNew(NEW_GAME_CENTER)->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(NEW_GAME_RIGHT1)->x = Lerp(Games_GetNew(NEW_GAME_RIGHT1)->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(NEW_GAME_RIGHT2)->x = Lerp(Games_GetNew(NEW_GAME_RIGHT2)->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        //Update x coordinates for old games
        Games_GetDisplayed(GAME_LEFT2)->x = Lerp(Games_GetDisplayed(GAME_LEFT2)->x, LEFT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->x = Lerp(Games_GetDisplayed(GAME_LEFT1)->x, LEFT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->x = Lerp(Games_GetDisplayed(CURRENT_GAME)->x, CENTER_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->x = Lerp(Games_GetDisplayed(GAME_RIGHT1)->x, RIGHT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->x = Lerp(Games_GetDisplayed(GAME_RIGHT2)->x, RIGHT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
    }
    //If scrolling to the left
    else {
        //If images close enough to final spot
        if (fabsf(Games_GetNew(NEW_GAME_CENTER)->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD) {
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
        //Update x coordinates for new games
        Games_GetNew(NEW_GAME_LEFT2)->x = Lerp(Games_GetNew(NEW_GAME_LEFT2)->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(NEW_GAME_LEFT1)->x = Lerp(Games_GetNew(NEW_GAME_LEFT1)->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(NEW_GAME_CENTER)->x = Lerp(Games_GetNew(NEW_GAME_CENTER)->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(NEW_GAME_RIGHT1)->x = Lerp(Games_GetNew(NEW_GAME_RIGHT1)->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(NEW_GAME_RIGHT2)->x = Lerp(Games_GetNew(NEW_GAME_RIGHT2)->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        //Update x coordinates for old games
        Games_GetDisplayed(GAME_LEFT2)->x = Lerp(Games_GetDisplayed(GAME_LEFT2)->x, LEFT2_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->x = Lerp(Games_GetDisplayed(GAME_LEFT1)->x, LEFT1_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->x = Lerp(Games_GetDisplayed(CURRENT_GAME)->x, CENTER_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->x = Lerp(Games_GetDisplayed(GAME_RIGHT1)->x, RIGHT1_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->x = Lerp(Games_GetDisplayed(GAME_RIGHT2)->x, RIGHT2_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
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
static void UI_DrawGames_Left(void) {
    UI_DrawGame(GAME_LEFT3, 1.0f, false);
    UI_DrawGame(GAME_LEFT2, 1.0f, false);
    UI_DrawGame(GAME_RIGHT2, 1.0f, false);
    UI_DrawGame(GAME_RIGHT1, 1.0f, false);
    UI_DrawGame(CURRENT_GAME, 1.0f, false);
    UI_DrawGame(GAME_LEFT1, 1.0f, false);
}

//Draw the games when scrolling to the right
static void UI_DrawGames_Right(void) {
    UI_DrawGame(GAME_RIGHT3, 1.0f, false);
    UI_DrawGame(GAME_LEFT2, 1.0f, false);
    UI_DrawGame(GAME_RIGHT2, 1.0f, false);
    UI_DrawGame(GAME_LEFT1, 1.0f, false);
    UI_DrawGame(CURRENT_GAME, 1.0f, false);
    UI_DrawGame(GAME_RIGHT1, 1.0f, false);
}

//Draw games when not scrolling
static void UI_DrawGames_Normal(void) {
    //Draw new games
    if (Var_GetScrollCateg() != SCROLL_NO) {
        UI_DrawGame(GAME_LEFT3, alphaCategories_In, true);
        UI_DrawGame(GAME_RIGHT1, alphaCategories_In, true);
        UI_DrawGame(GAME_LEFT2, alphaCategories_In, true);
        UI_DrawGame(CURRENT_GAME, alphaCategories_In, true);
        UI_DrawGame(GAME_LEFT1, alphaCategories_In, true);
    }
    UI_DrawGame(GAME_LEFT2, alphaCategories_Out, false);
    UI_DrawGame(GAME_RIGHT2, alphaCategories_Out, false);
    UI_DrawGame(GAME_LEFT1, alphaCategories_Out, false);
    UI_DrawGame(GAME_RIGHT1, alphaCategories_Out, false);
    UI_DrawGame(CURRENT_GAME, alphaCategories_Out, false);
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
static void UI_DrawGames_Scroll(void) {
    //If scrolling to the right
    if (Var_GetScrollGames() == SCROLL_RIGHT) {
        //Draw the games
        UI_DrawGames_Right();
        //If images close enough to final spot
        if (fabsf(Games_GetDisplayed(GAME_RIGHT1)->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD) {
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
        Games_GetDisplayed(GAME_LEFT2)->x = Lerp(Games_GetDisplayed(GAME_LEFT2)->x, LEFT3_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->x = Lerp(Games_GetDisplayed(GAME_LEFT1)->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->x = Lerp(Games_GetDisplayed(CURRENT_GAME)->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->x = Lerp(Games_GetDisplayed(GAME_RIGHT1)->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->x = Lerp(Games_GetDisplayed(GAME_RIGHT2)->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT3)->x = Lerp(Games_GetDisplayed(GAME_RIGHT3)->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        //Update y coordinates
        Games_GetDisplayed(GAME_LEFT2)->y = Lerp(Games_GetDisplayed(GAME_LEFT2)->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->y = Lerp(Games_GetDisplayed(GAME_LEFT1)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->y = Lerp(Games_GetDisplayed(CURRENT_GAME)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->y = Lerp(Games_GetDisplayed(GAME_RIGHT1)->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->y = Lerp(Games_GetDisplayed(GAME_RIGHT2)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT3)->y = Lerp(Games_GetDisplayed(GAME_RIGHT3)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        //Update width values
        Games_GetDisplayed(GAME_LEFT2)->w = Lerp(Games_GetDisplayed(GAME_LEFT2)->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->w = Lerp(Games_GetDisplayed(GAME_LEFT1)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->w = Lerp(Games_GetDisplayed(CURRENT_GAME)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->w = Lerp(Games_GetDisplayed(GAME_RIGHT1)->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->w = Lerp(Games_GetDisplayed(GAME_RIGHT2)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT3)->w = Lerp(Games_GetDisplayed(GAME_RIGHT3)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        //Update height values
        Games_GetDisplayed(GAME_LEFT2)->h = Lerp(Games_GetDisplayed(GAME_LEFT2)->h, SIDE3_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->h = Lerp(Games_GetDisplayed(GAME_LEFT1)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->h = Lerp(Games_GetDisplayed(CURRENT_GAME)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->h = Lerp(Games_GetDisplayed(GAME_RIGHT1)->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->h = Lerp(Games_GetDisplayed(GAME_RIGHT2)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT3)->h = Lerp(Games_GetDisplayed(GAME_RIGHT3)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
    }
    //If scrolling to the left
    else if (Var_GetScrollGames() == SCROLL_LEFT) {
        //Draw the games
        UI_DrawGames_Left();
        //If images close enough to final spot
        if (fabsf(Games_GetDisplayed(GAME_LEFT1)->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD) {
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
        Games_GetDisplayed(GAME_LEFT3)->x = Lerp(Games_GetDisplayed(GAME_LEFT3)->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT2)->x = Lerp(Games_GetDisplayed(GAME_LEFT2)->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->x = Lerp(Games_GetDisplayed(GAME_LEFT1)->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->x = Lerp(Games_GetDisplayed(CURRENT_GAME)->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->x = Lerp(Games_GetDisplayed(GAME_RIGHT1)->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->x = Lerp(Games_GetDisplayed(GAME_RIGHT2)->x, RIGHT3_GAME_X, SCROLL_GAMES_SPEED);
        //Update y coordinates
        Games_GetDisplayed(GAME_LEFT3)->y = Lerp(Games_GetDisplayed(GAME_LEFT3)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT2)->y = Lerp(Games_GetDisplayed(GAME_LEFT2)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->y = Lerp(Games_GetDisplayed(GAME_LEFT1)->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->y = Lerp(Games_GetDisplayed(CURRENT_GAME)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->y = Lerp(Games_GetDisplayed(GAME_RIGHT1)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->y = Lerp(Games_GetDisplayed(GAME_RIGHT2)->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        //Update width values
        Games_GetDisplayed(GAME_LEFT3)->w = Lerp(Games_GetDisplayed(GAME_LEFT3)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT2)->w = Lerp(Games_GetDisplayed(GAME_LEFT2)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->w = Lerp(Games_GetDisplayed(GAME_LEFT1)->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->w = Lerp(Games_GetDisplayed(CURRENT_GAME)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->w = Lerp(Games_GetDisplayed(GAME_RIGHT1)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->w = Lerp(Games_GetDisplayed(GAME_RIGHT2)->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        //Update height values
        Games_GetDisplayed(GAME_LEFT3)->h = Lerp(Games_GetDisplayed(GAME_LEFT3)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT2)->h = Lerp(Games_GetDisplayed(GAME_LEFT2)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_LEFT1)->h = Lerp(Games_GetDisplayed(GAME_LEFT1)->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(CURRENT_GAME)->h = Lerp(Games_GetDisplayed(CURRENT_GAME)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT1)->h = Lerp(Games_GetDisplayed(GAME_RIGHT1)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(GAME_RIGHT2)->h = Lerp(Games_GetDisplayed(GAME_RIGHT2)->h, SIDE3_GAME_H, SCROLL_GAMES_SPEED);
    }
}

//Init function for the main UI
void UI_Init(void) {
    alphaGames = 1.0f;
    alphaCategories_Out = 1.0f;
    alphaCategories_In = 0.0f;
    alphaSelectBox = 1.0f;
    alphaSelectTxt_TimeElapsed = 0.0f;
    alphaSelectTxt_Blink = true;
    UI_ResetCoords_Games();
    currentUIState = NORMAL;
}

//Tick function for the main UI
void UI_Tick(ConsoleState* currentConsoleState) {
    //Transition
    switch (currentUIState) {
        //Normal, static menu
        case NORMAL:
            //If not scrolling games to left, not scrolling categories, and the right input is pressed
            if (Var_GetScrollGames() != SCROLL_LEFT && Var_GetScrollCateg() == SCROLL_NO && (IsKeyDown(KEY_RIGHT) || LS_RIGHT)) {
                Var_SetScrollGames(SCROLL_RIGHT);
                currentUIState = SCROLL_GAMES;
            }
            //If not scrolling games to right, not scrolling categories, and the right input is pressed
            else if (Var_GetScrollGames() != SCROLL_RIGHT && Var_GetScrollCateg() == SCROLL_NO && (IsKeyDown(KEY_LEFT) || LS_LEFT)) {
                Var_SetScrollGames(SCROLL_LEFT);
                currentUIState = SCROLL_GAMES;
            }
            //If not scrolling categories to left and right input is pressed
            if (Var_GetScrollCateg() != SCROLL_LEFT && (IsKeyDown(KEY_D) || RB_DOWN)) {
                //If not already scrolling categories
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
                //If not already scrolling categories
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
            //Pause if the correct button is pressed
            if (IsKeyPressed(KEY_ESCAPE) || (HOME_PRESS && !Controller_GetWasPressed_Home())) {
                UIPause_Init();
                currentUIState = OPTIONS;
            }
            //Go to erase data pop-up if correct button is pressed
            else if (IsKeyPressed(KEY_P) || START_PRESS) {
                Erase_Init();
                currentUIState = ERASE_DATA;
            }
            break;

        //Scrolling games
        case SCROLL_GAMES:
            //If not scrolling games anymore, transition
            if (Var_GetScrollGames() == SCROLL_NO) {
                currentUIState = NORMAL;
                alphaSelectTxt_Blink = true;
                alphaSelectTxt_TimeElapsed = 0.0f;
            }
            //Pause if the correct button is pressed
            if (IsKeyPressed(KEY_ESCAPE) || HOME_PRESS) {
                UIPause_Init();
                currentUIState = OPTIONS;
            }
            //Start scrolling categories if right button is pressed
            if (IsKeyPressed(KEY_A) || LB_PRESS) {
                Var_SetScrollGames(SCROLL_NO);
                Games_ScrollLeft();
                UI_ResetCoords_Games();
                Var_SetScrollCateg(SCROLL_LEFT);
                currentUIState = SCROLL_CATEGORIES;
                Games_UpdateNewIndexes(LEFT);
                Games_NewRefresh();
                UI_ResetCoords_Categ();
            }
            else if (IsKeyPressed(KEY_D) || RB_PRESS) {
                Var_SetScrollGames(SCROLL_NO);
                Games_ScrollRight();
                UI_ResetCoords_Games();
                Var_SetScrollCateg(SCROLL_RIGHT);
                currentUIState = SCROLL_CATEGORIES;
                Games_UpdateNewIndexes(RIGHT);
                Games_NewRefresh();
                UI_ResetCoords_Categ();
            }
            break;

        //Scrolling categories
        case SCROLL_CATEGORIES:
        //If not scrolling anymore, stop
            if (Var_GetScrollCateg() == SCROLL_NO) {
                currentUIState = NORMAL;
                alphaSelectTxt_Blink = true;
                alphaSelectTxt_TimeElapsed = 0.0f;
            }
            //Pause if the correct button is pressed
            if (IsKeyPressed(KEY_ESCAPE) || HOME_PRESS) {
                UIPause_Init();
                currentUIState = OPTIONS;
            }
            break;

        //Options menu 
        case OPTIONS:
            //Go back to normal UI screen if right button is pressed
            if (HOME_PRESS || ((IsKeyPressed(KEY_ESCAPE) || B_PRESS) && !Var_GetDisplayBright() && !Var_GetDisplayTheme())) {
                currentUIState = NORMAL;
                Controller_SetWasPressed_B(true);
            }
            break;

        //Erase data pop-up
        case ERASE_DATA:
            break;
    }

    //Action
    switch (currentUIState) {
        //Normal, static menu
        case NORMAL:
            UI_ChangeAlpha_Static();
            UI_DrawBase();
            UI_DrawCateg_Static();
            UI_DrawGames_Normal();
            UI_DrawTop();
            UI_DrawBottom();
            UI_DrawDispDiag(false);
            //If game is pressed, launch the game
            if ((IsKeyPressed(KEY_ENTER) || A_PRESS) && !Controller_GetWasPressed_A()) {
                *currentConsoleState = STATE_APP_LAUNCHER;
                Controller_SetWasPressed_A(true);
            }
            break;
        
        //Scrolling games
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

        //Scrolling categories
        case SCROLL_CATEGORIES:
            UI_ChangeAlpha_ScrollCateg();
            UI_DrawBase();
            UI_DrawCateg_Scroll();
            UI_DrawGames_Normal();
            UI_DrawTop();
            UI_DrawBottom();
            UI_DrawDispDiag(false);
            break;

        //Options menu
        case OPTIONS:
            UI_DrawBase();
            //If scrolling games
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
            //If scrolling categories
            else if (Var_GetScrollCateg() != SCROLL_NO) {
                UI_ChangeAlpha_ScrollCateg();
                UI_DrawCateg_Scroll();
                UI_DrawGames_Normal();
            }
            //If static
            else {
                UI_ChangeAlpha_Static();
                UI_DrawCateg_Static();
                UI_DrawGames_Normal();
            }
            UI_DrawTop();
            UI_DrawBottom();
            UI_DrawDispDiag(false);
            UIPause_Tick(currentConsoleState);
            break;
        
        //Erase data pop-up
        case ERASE_DATA:
            UI_DrawBase();
            UI_ChangeAlpha_Static();
            UI_DrawCateg_Static();
            UI_DrawGames_Normal();
            UI_DrawTop();
            UI_DrawBottom();
            UI_DrawDispDiag(false);
            Erase_Tick(Games_GetDisplayed(CURRENT_GAME), &currentUIState);
            break;
    }
}
