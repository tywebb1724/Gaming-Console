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

mainMenuState currentMainMenuState = NORMAL;
optionsState currentOptionsState = LIST;
screenState currentScreenState = OPTIONS;
diagState currentDiagState = DIAG_NORMAL;
pausedState currentPausedState = RESTART;
float optionsTimeElapsed = 0.0f;
bool openDiagnostics = false;
bool displayBrightness = false;
bool displayTheme = false;
bool displayControls = false;
brightPressed currentBrightPressed;

float debugTimer = 0.0f;

static float maxLen;

char brightnessPercent_Txt[5];

bool mouseLeftWasPressed = false;
bool mouseRightWasPressed = false;

static int monitorWidth;
static int monitorHeight;

static Texture2D spiderLogo;

static Texture2D background_Blue;
static Texture2D background_Red;
static Texture2D background_Green;
static Texture2D background_Yellow;
static Texture2D currentBackground;
static Texture2D controls;



// Types of font
Font fontRegular;
Font fontBold;
// Variable to hold the current image to draw
Texture2D img;
// Variables to hold information about the current image to draw
int img_X, img_Y, img_W, img_H;
// Different alphas
float alphaGames;
float alphaCategories_Out;
float alphaCategories_In;
float alphaSelectBox;
float alphaSelectTxt = 0.0f;
float alphaSelectTxt_TimeElapsed = 0.0f;
bool alphaSelectTxt_Blink = true;


theme_t currentTheme;
theme_t currentThemeSelected;
float themeTimeElapsed;

//Extract a color from the color name
static const Color NametoColor(char *c) {
    //Return the corresponding color for each name
    if (strcmp(c, "BLUE") == 0) {
        return BLUE;
    }
    else if (strcmp(c, "RED") == 0) {
        return RED;
    }
    else if (strcmp(c, "BLACK") == 0) {
        return BLACK;
    }
    else if (strcmp(c, "WHITE") == 0) {
        return WHITE;
    }
    else if (strcmp(c, "GREEN") == 0) {
        return GREEN;
    }
    else if (strcmp(c, "YELLOW") == 0) {
        return YELLOW;
    }
    //Fallback case
    return BLUE;
}

//Extract a background from a color name
static const Texture2D NametoBackground(char *c) {
    //Return the corresponding background for each color name
    if (strcmp(c, "BLUE") == 0) {
        return background_Blue;
    }
    else if (strcmp(c, "RED") == 0) {
        return background_Red;
    }
    else if (strcmp(c, "GREEN") == 0) {
        return background_Green;
    }
    else if (strcmp(c, "YELLOW") == 0) {
        return background_Yellow;
    }
    //Fallback case
    return background_Blue;
}

//Extract a name from the color
static const char *ColorToName(Color c) {
    //Return the corresponding name depending on rgba values
    if (c.r == BLUE.r && c.g == BLUE.g && c.b == BLUE.b && c.a == BLUE.a)
        return "BLUE";
    if (c.r == RED.r && c.g == RED.g && c.b == RED.b && c.a == RED.a)
        return "RED";
    if (c.r == BLACK.r && c.g == BLACK.g && c.b == BLACK.b && c.a == BLACK.a)
        return "BLACK";
    if (c.r == GREEN.r && c.g == GREEN.g && c.b == GREEN.b && c.a == GREEN.a)
        return "GREEN";
    if (c.r == YELLOW.r && c.g == YELLOW.g && c.b == YELLOW.b && c.a == YELLOW.a)
        return "YELLOW";
    if (c.r == WHITE.r && c.g == WHITE.g && c.b == WHITE.b && c.a == WHITE.a)
        return "WHITE";
    //Fallback case
    return "UNKNOWN";
}

//Update the UI text file
static void Update_UI_Txt() {
    //Open the file
    FILE *f = fopen("/home/tywebb1724/Desktop/Gaming-Console/assets/system/ui.txt", "w");
    //If it opens correctly, write the variables
    if (f) {
        fprintf(f, "%s\n", ColorToName(Var_GetColor1()));
        fprintf(f, "%s\n", ColorToName(Var_GetColor2()));
        fprintf(f, "%s\n", ColorToName(Var_GetColor3()));
        fprintf(f, "%f\n", Var_GetBright());
        fprintf(f, "%f\n", Var_GetBrightX());
        fprintf(f, "%d\n", displayDiag);
        fclose(f);
    }
}





//////////////////
void UI_ChangeAlpha_Normal()
{
    if (alphaSelectTxt_Blink == true)
    {
        // If less than 1, increment
        if (alphaSelectTxt < 1.0f)
        {
            alphaSelectTxt += 0.04f;
        }
        else if (alphaSelectTxt >= 1.0f)
        {
            alphaSelectTxt_TimeElapsed += GetFrameTime();
            if (alphaSelectTxt_TimeElapsed >= 1)
            {
                alphaSelectTxt_Blink = false;
                alphaSelectTxt_TimeElapsed = 0.0f;
            }
        }
    }
    else
    {
        // If less than 1, increment
        if (alphaSelectTxt > 0.1f)
        {
            alphaSelectTxt -= 0.04f;
        }
        // If greater than 1, bring back to 1
        else if (alphaSelectTxt <= 0.1f)
        {
            alphaSelectTxt_TimeElapsed += GetFrameTime();
            if (alphaSelectTxt_TimeElapsed >= 0.1)
            {
                alphaSelectTxt_Blink = true;
                alphaSelectTxt_TimeElapsed = 0.0f;
            }
        }
    }
    // If less than 1, increment
    if (alphaGames < 1.0f)
    {
        alphaGames += 0.25f;
    }
    // If greater than 1, bring back to 1
    else if (alphaGames > 1.0f)
    {
        alphaGames = 1.0f;
    }
    if (alphaSelectBox < 1.0f)
    {
        alphaSelectBox += 0.25f;
    }
    else if (alphaSelectBox > 1.0f)
    {
        alphaSelectBox = 1.0f;
    }
}

void UI_ChangeAlpha_ScrollGames()
{
    // If less than 0, bring to 0
    if (alphaSelectTxt < 0.0f)
    {
        alphaSelectTxt = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaSelectTxt > 0.0f)
    {
        alphaSelectTxt -= 0.25f;
    }
    // If less than 0, bring to 0
    if (alphaGames < 0.0f)
    {
        alphaGames = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaGames > 0.0f)
    {
        alphaGames -= 0.25f;
    }
    if (alphaSelectBox < 1.0f)
    {
        alphaSelectBox += 0.25f;
    }
    else if (alphaSelectBox > 1.0f)
    {
        alphaSelectBox = 1.0f;
    }
}

void UI_ChangeAlpha_ScrollCategories()
{
    // If less than 0, bring to 0
    if (alphaSelectTxt < 0.0f)
    {
        alphaSelectTxt = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaSelectTxt > 0.0f)
    {
        alphaSelectTxt -= 0.25f;
    }
    // If less than 0, bring to 0
    if (alphaGames < 0.0f)
    {
        alphaGames = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaGames > 0.0f)
    {
        alphaGames -= 0.25f;
    }
    if (alphaSelectBox > 0.0f)
    {
        alphaSelectBox -= 0.25f;
    }
    else if (alphaSelectBox < 0.0f)
    {
        alphaSelectBox = 0.0f;
    }
    // If greater than 1, bring to 1
    if (alphaCategories_In > 1.0f)
    {
        alphaCategories_In = 1.0f;
    }
    // If less than 1, increment up
    else if (alphaCategories_In < 1.0f)
    {
        alphaCategories_In += 0.05f;
    }
    // If less than 0, bring to 0
    if (alphaCategories_Out < 0.0f)
    {
        alphaCategories_Out = 0.0f;
    }
    // If greater than 0, decrement
    else if (alphaCategories_Out > 0.0f)
    {
        alphaCategories_Out -= 0.1f;
    }
}

///////



// Reset the coordinates for the displayed games after games scroll
void UI_ResetDisplayCoords_Games() {
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

//Draw an image
void UI_DrawImage(float alpha) {
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
void UI_DrawOptionsBtn() {
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
    Vector2 optionsSize = MeasureTextEx(fontBold, OPTIONS_TXT, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE);
    Vector2 options = {OPTIONS_TXT_X, OPTIONS_TXT_Y};
    DrawTextEx(fontRegular, OPTIONS_TXT, options, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE, Var_GetColor3());
}

//Draw bumpers, option button, and small spider logo
void UI_DrawTop() {
    UI_DrawBumpers();
    UI_DrawOptionsBtn();
    //Draw small spider logo
    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(SMALL_LOGO_SIZE, CENTER_X), UI_CenterImg_Y(SMALL_LOGO_SIZE, SMALL_LOGO_Y), SMALL_LOGO_SIZE, SMALL_LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Draw the select controls at the bottom
void UI_DrawBottom() {
    //Draw left arrow (LS)
    DrawCircle(LS_LEFT_X + LS_OFFSET, LS_Y, LS_RADIUS, Fade(Var_GetColor3(), alphaSelectTxt));
    Vector2 point1 = {LS_LEFT_X - (LS_TRIANGLE_SIZE / 2), LS_Y};
    Vector2 point2 = {LS_LEFT_X + (LS_TRIANGLE_SIZE / 2), LS_Y + LS_TRIANGLE_SIZE / 2};
    Vector2 point3 = {LS_LEFT_X + (LS_TRIANGLE_SIZE / 2), LS_Y - LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point1, point2, point3, Fade(Var_GetColor2(), alphaSelectTxt));
    //Draw right arrow (RS)
    DrawCircle(LS_RIGHT_X - LS_OFFSET, LS_Y, LS_RADIUS, Fade(Var_GetColor3(), alphaSelectTxt));
    Vector2 point4 = {LS_RIGHT_X + (LS_TRIANGLE_SIZE / 2), LS_Y};
    Vector2 point5 = {LS_RIGHT_X - (LS_TRIANGLE_SIZE / 2), LS_Y + LS_TRIANGLE_SIZE / 2};
    Vector2 point6 = {LS_RIGHT_X - (LS_TRIANGLE_SIZE / 2), LS_Y - LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point6, point5, point4, Fade(Var_GetColor2(), alphaSelectTxt));
    //Values for Text
    Vector2 gameSize = MeasureTextEx(fontBold, Games_GetDisplayed(3)->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 bottomSize = MeasureTextEx(fontBold, BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 consoleSize = MeasureTextEx(fontBold, Games_GetDisplayed(3)->console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 game = {BOTTOM_TXT_X, GAME_TXT_Y};
    Vector2 bottom = {BOTTOM_TXT_X, BOTTOM_TXT_Y};
    Vector2 console = {BOTTOM_TXT_X, CONSOLE_TXT_Y};
    //Draw section
    Rectangle rect = {
        BOTTOM_RECT_X,
        BOTTOM_RECT_Y,
        BOTTOM_RECT_W,
        BOTTOM_RECT_H};

    DrawRectangleRoundedLinesEx(rect, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_BOTTOM, Fade(Var_GetColor3(), alphaSelectBox));
    DrawRectangleRounded(rect, BOTTOM_ROUND, BOTTOM_SEGMENTS, Fade(Var_GetColor2(), alphaSelectBox));
    //Draw text
    DrawTextEx(fontBold, Games_GetDisplayed(3)->title, game, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaSelectTxt));
    DrawTextEx(fontBold, Games_GetDisplayed(3)->console, console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaSelectTxt));
    DrawTextEx(fontBold, BOTTOM_TXT, bottom, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaSelectTxt));
}

// Function for drawing the boot screen
void UI_DrawBootScreen() {
    //Clear background
    ClearBackground(BACKGROUND_CLR);
    //Draw logo
    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(LOGO_SIZE, CENTER_X), UI_CenterImg_Y(LOGO_SIZE, SCREEN_H / 2), LOGO_SIZE, LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Draw launching screen
void UI_DrawLaunch(game_t game) {
    //Clear background
    ClearBackground(BLACK);
    //Launching text
    Vector2 launchTxt_Size = MeasureTextEx(fontRegular, LAUNCH_TXT, LAUNCH_TXT_SIZE, LAUNCH_SPACE);
    Vector2 launchTxt_Pos = {LAUNCH_TITLE_X, LAUNCH_TITLE_Y};
    DrawTextEx(fontRegular, LAUNCH_TXT, launchTxt_Pos, LAUNCH_TXT_SIZE, LAUNCH_SPACE, WHITE);
    //Name of game
    Vector2 gameTxt_Size = MeasureTextEx(fontBold, game.title, LAUNCH_TXT_SIZE, LAUNCH_SPACE);
    Vector2 gameTxt_Pos = {LAUNCH_GAME_X, LAUNCH_GAME_Y};
    DrawTextEx(fontBold, game.title, gameTxt_Pos, LAUNCH_TXT_SIZE, LAUNCH_SPACE, WHITE);
    //Home instructions
    Vector2 homeTxt_Size = MeasureTextEx(fontRegular, LAUNCH_HOME, LAUNCH_TXT_SIZE, LAUNCH_SPACE);
    Vector2 homeTxt_Pos = {LAUNCH_HOME_X, LAUNCH_HOME_Y};
    DrawTextEx(fontRegular, LAUNCH_HOME, homeTxt_Pos, LAUNCH_TXT_SIZE, LAUNCH_SPACE, WHITE);
}



//Function to draw the bumpers
void UI_DrawBumpers() {
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
    Vector2 bumperLSize = MeasureTextEx(fontBold, BUMPER_L_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperRSize = MeasureTextEx(fontBold, BUMPER_R_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperR = {BUMPER_R_TXT_X, BUMPER_TXT_Y};
    Vector2 bumperL = {BUMPER_L_TXT_X, BUMPER_TXT_Y};

    //Draw bumpers
    if (Var_GetScrollCateg() == SCROLL_RIGHT) {
        DrawRectangleRounded(bumperLRec, BUMPER_ROUND, BUMPER_SEGMENTS, Fade(Var_GetColor2(), alphaCategories_Out));
        DrawRectangleRounded(bumperRRec, BUMPER_ROUND, BUMPER_SEGMENTS, Var_GetColor3());
        DrawTextEx(fontBold, BUMPER_R_TXT, bumperR, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Var_GetColor2());
        DrawTextEx(fontBold, BUMPER_L_TXT, bumperL, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Fade(Var_GetColor2(), alphaCategories_Out));
    }
    else if (Var_GetScrollCateg() == SCROLL_LEFT) {
        DrawRectangleRounded(bumperLRec, BUMPER_ROUND, BUMPER_SEGMENTS, Var_GetColor3());
        DrawRectangleRounded(bumperRRec, BUMPER_ROUND, BUMPER_SEGMENTS, Fade(Var_GetColor2(), alphaCategories_Out));
        DrawTextEx(fontBold, BUMPER_R_TXT, bumperR, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Fade(Var_GetColor2(), alphaCategories_Out));
        DrawTextEx(fontBold, BUMPER_L_TXT, bumperL, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Var_GetColor2());
    }
    else {
        DrawRectangleRounded(bumperLRec, BUMPER_ROUND, BUMPER_SEGMENTS, Var_GetColor3());
        DrawRectangleRounded(bumperRRec, BUMPER_ROUND, BUMPER_SEGMENTS, Var_GetColor3());
        DrawTextEx(fontBold, BUMPER_R_TXT, bumperR, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Var_GetColor2());
        DrawTextEx(fontBold, BUMPER_L_TXT, bumperL, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Var_GetColor2());
    }
}



// Reset the coordinates for the displayed games after category scroll
void UI_ResetDisplayCoords_Scroll()
{
    // If scrolling to the right
    if (Var_GetScrollCateg() == SCROLL_RIGHT)
    {
        // Reset x coordinates
        Games_GetNew(0)->x = LEFT2_SCROLLL_X;
        Games_GetNew(1)->x = LEFT1_SCROLLL_X;
        Games_GetNew(2)->x = CENTER_SCROLLL_X;
        Games_GetNew(3)->x = RIGHT1_SCROLLL_X;
        Games_GetNew(4)->x = RIGHT2_SCROLLL_X;
    }
    // If scrolling to the left
    else
    {
        // Reset x coordinates
        Games_GetNew(0)->x = LEFT2_SCROLLR_X;
        Games_GetNew(1)->x = LEFT1_SCROLLR_X;
        Games_GetNew(2)->x = CENTER_SCROLLR_X;
        Games_GetNew(3)->x = RIGHT1_SCROLLR_X;
        Games_GetNew(4)->x = RIGHT2_SCROLLR_X;
    }
    // Reset y coordinates
    Games_GetNew(0)->y = SIDE2_GAME_Y;
    Games_GetNew(1)->y = SIDE1_GAME_Y;
    Games_GetNew(2)->y = CENTER_GAME_Y;
    Games_GetNew(3)->y = SIDE1_GAME_Y;
    Games_GetNew(4)->y = SIDE2_GAME_Y;
    // Reset width values
    Games_GetNew(0)->w = SIDE2_GAME_W;
    Games_GetNew(1)->w = SIDE1_GAME_W;
    Games_GetNew(2)->w = CENTER_GAME_W;
    Games_GetNew(3)->w = SIDE1_GAME_W;
    Games_GetNew(4)->w = SIDE2_GAME_W;
    // Reset height values
    Games_GetNew(0)->h = SIDE2_GAME_H;
    Games_GetNew(1)->h = SIDE1_GAME_H;
    Games_GetNew(2)->h = CENTER_GAME_H;
    Games_GetNew(3)->h = SIDE1_GAME_H;
    Games_GetNew(4)->h = SIDE2_GAME_H;
}


// Function for drawing a game cover
void UI_DrawGame(int i, float alpha, bool scroll)
{
    // If scrolling
    if (scroll)
    {
        // Update values of image to draw
        img_Y = Games_GetNew(i)->y;
        img_W = Games_GetNew(i)->w;
        img_H = Games_GetNew(i)->h;
        img_X = Games_GetNew(i)->x;
        img = Games_GetNew(i)->cover;
        // Draw image
        UI_DrawImage(alpha);
        // Draw outline
        DrawRectangleLinesEx(
            (Rectangle){
                img_X - THICKNESS_OTHER,
                img_Y - THICKNESS_OTHER,
                img_W + (2 * THICKNESS_OTHER),
                img_H + (2 * THICKNESS_OTHER)},
            THICKNESS_OTHER,
            Fade(GRAY, alphaCategories_In));
    }
    // If not scrolling
    else
    {
        // Update values of image to draw
        img_Y = Games_GetDisplayed(i)->y;
        img_W = Games_GetDisplayed(i)->w;
        img_H = Games_GetDisplayed(i)->h;
        img_X = Games_GetDisplayed(i)->x;
        img = Games_GetDisplayed(i)->cover;
        // Draw image
        UI_DrawImage(alpha);
        // Draw outline
        DrawRectangleLinesEx(
            (Rectangle){
                img_X - THICKNESS_OTHER,
                img_Y - THICKNESS_OTHER,
                img_W + (2 * THICKNESS_OTHER),
                img_H + (2 * THICKNESS_OTHER)},
            THICKNESS_OTHER,
            Fade(GRAY, alphaCategories_Out));
    }
}

// Function for drawing the games when scrolling to the left
void UI_DrawGames_Left()
{
    UI_DrawGame(0, 1.0f, false);
    UI_DrawGame(1, 1.0f, false);
    UI_DrawGame(5, 1.0f, false);
    UI_DrawGame(4, 1.0f, false);
    UI_DrawGame(3, 1.0f, false);
    UI_DrawGame(2, 1.0f, false);
}

// Function for drawing the games when scrolling to the right
void UI_DrawGames_Right()
{
    UI_DrawGame(6, 1.0f, false);
    UI_DrawGame(1, 1.0f, false);
    UI_DrawGame(5, 1.0f, false);
    UI_DrawGame(2, 1.0f, false);
    UI_DrawGame(3, 1.0f, false);
    UI_DrawGame(4, 1.0f, false);
}

// Function for drawing the animation of scrolling games
void UI_DrawScroll_Games()
{
    // If scrolling to the right
    if (Var_GetScrollGames() == SCROLL_RIGHT)
    {
        // Draw the games
        UI_DrawGames_Right();
        // If images close enough to final spot
        if (fabsf(Games_GetDisplayed(4)->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD)
        {
            if ((!IsKeyDown(KEY_RIGHT) && !LS_RIGHT) || currentMainMenuState == OPTIONS)
            {
                Var_SetScrollGames(SCROLL_NO);
            }
            // Update displayed games
            Games_ScrollRight();
            // Reset game values
            UI_ResetDisplayCoords_Games();
            return;
        }
        // Update x coordinates
        Games_GetDisplayed(1)->x = Lerp(Games_GetDisplayed(1)->x, LEFT3_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->x = Lerp(Games_GetDisplayed(2)->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->x = Lerp(Games_GetDisplayed(3)->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->x = Lerp(Games_GetDisplayed(4)->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->x = Lerp(Games_GetDisplayed(5)->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(6)->x = Lerp(Games_GetDisplayed(6)->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        // Update y coordinates
        Games_GetDisplayed(1)->y = Lerp(Games_GetDisplayed(1)->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->y = Lerp(Games_GetDisplayed(2)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->y = Lerp(Games_GetDisplayed(3)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->y = Lerp(Games_GetDisplayed(4)->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->y = Lerp(Games_GetDisplayed(5)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(6)->y = Lerp(Games_GetDisplayed(6)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        // Update width values
        Games_GetDisplayed(1)->w = Lerp(Games_GetDisplayed(1)->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->w = Lerp(Games_GetDisplayed(2)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->w = Lerp(Games_GetDisplayed(3)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->w = Lerp(Games_GetDisplayed(4)->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->w = Lerp(Games_GetDisplayed(5)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(6)->w = Lerp(Games_GetDisplayed(6)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        // Update height values
        Games_GetDisplayed(1)->h = Lerp(Games_GetDisplayed(1)->h, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->h = Lerp(Games_GetDisplayed(2)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->h = Lerp(Games_GetDisplayed(3)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->h = Lerp(Games_GetDisplayed(4)->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->h = Lerp(Games_GetDisplayed(5)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(6)->h = Lerp(Games_GetDisplayed(6)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
    }
    // If scrolling to the left
    else if (Var_GetScrollGames() == SCROLL_LEFT)
    {
        // Draw the games
        UI_DrawGames_Left();
        // If images close enough to final spot
        if (fabsf(Games_GetDisplayed(2)->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD)
        {
            if ((!IsKeyDown(KEY_LEFT) && !LS_LEFT) || currentMainMenuState == OPTIONS)
            {
                Var_SetScrollGames(SCROLL_NO);
            }
            // Update displayed games
            Games_ScrollLeft();
            // Reset game values
            UI_ResetDisplayCoords_Games();
            return;
        }
        // Update x coordinates
        Games_GetDisplayed(0)->x = Lerp(Games_GetDisplayed(0)->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(1)->x = Lerp(Games_GetDisplayed(1)->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->x = Lerp(Games_GetDisplayed(2)->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->x = Lerp(Games_GetDisplayed(3)->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->x = Lerp(Games_GetDisplayed(4)->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->x = Lerp(Games_GetDisplayed(5)->x, RIGHT3_GAME_X, SCROLL_GAMES_SPEED);
        // Update y coordinates
        Games_GetDisplayed(0)->y = Lerp(Games_GetDisplayed(0)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(1)->y = Lerp(Games_GetDisplayed(1)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->y = Lerp(Games_GetDisplayed(2)->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->y = Lerp(Games_GetDisplayed(3)->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->y = Lerp(Games_GetDisplayed(4)->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->y = Lerp(Games_GetDisplayed(5)->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        // Update width values
        Games_GetDisplayed(0)->w = Lerp(Games_GetDisplayed(0)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(1)->w = Lerp(Games_GetDisplayed(1)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->w = Lerp(Games_GetDisplayed(2)->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->w = Lerp(Games_GetDisplayed(3)->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->w = Lerp(Games_GetDisplayed(4)->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->w = Lerp(Games_GetDisplayed(5)->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        // Update height values
        Games_GetDisplayed(0)->h = Lerp(Games_GetDisplayed(0)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(1)->h = Lerp(Games_GetDisplayed(1)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(2)->h = Lerp(Games_GetDisplayed(2)->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(3)->h = Lerp(Games_GetDisplayed(3)->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(4)->h = Lerp(Games_GetDisplayed(4)->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        Games_GetDisplayed(5)->h = Lerp(Games_GetDisplayed(5)->h, SIDE3_GAME_H, SCROLL_GAMES_SPEED);
    }
}

void MainMenu_DrawGames_Normal()
{
    // If scrolling categories
    if (Var_GetScrollCateg() != SCROLL_NO)
    {
        UI_DrawGame(0, alphaCategories_In, true);
        UI_DrawGame(4, alphaCategories_In, true);
        UI_DrawGame(1, alphaCategories_In, true);
        UI_DrawGame(3, alphaCategories_In, true);
        UI_DrawGame(2, alphaCategories_In, true);
    }
    // If not scrolling categories
    UI_DrawGame(1, alphaCategories_Out, false);
    UI_DrawGame(5, alphaCategories_Out, false);
    UI_DrawGame(2, alphaCategories_Out, false);
    UI_DrawGame(4, alphaCategories_Out, false);
    UI_DrawGame(3, alphaCategories_Out, false);

    // Draw outline for selected game
    DrawRectangleLinesEx(
        (Rectangle){
            img_X - THICKNESS_SELECT_GAME,
            img_Y - THICKNESS_SELECT_GAME,
            img_W + (2 * THICKNESS_SELECT_GAME),
            img_H + (2 * THICKNESS_SELECT_GAME)},
        THICKNESS_SELECT_GAME,
        Fade(Var_GetColor1(), alphaGames));
}

// Function for drawing the cateogires when they are static
void UI_DrawCategories_Normal()
{
    // Draw center category
    Vector2 size = MeasureTextEx(fontBold, Categories_GetDisplayed(2), CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 centerCateg = {CENTER_CATEG_X, CENTER_CATEG_Y};
    DrawTextEx(fontBold, Categories_GetDisplayed(2), centerCateg, CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaCategories_Out));

    // Draw left category
    size = MeasureTextEx(fontBold, Categories_GetDisplayed(1), SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 left1Categ = {LEFT1_CATEG_X, SIDE_CATEG_Y};
    DrawTextEx(fontBold, Categories_GetDisplayed(1), left1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaCategories_Out));
    // Draw right category
    size = MeasureTextEx(fontBold, Categories_GetDisplayed(3), SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 right1Categ = {RIGHT1_CATEG_X, SIDE_CATEG_Y};
    DrawTextEx(fontBold, Categories_GetDisplayed(3), right1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(Var_GetColor3(), alphaCategories_Out));
}

// Function for drawing the animation of scrolling categories
void UI_DrawScroll_Categories()
{
    // If scrolling to the right
    if (Var_GetScrollCateg() == SCROLL_RIGHT)
    {
        // If images close enough to final spot
        if (fabsf(Games_GetNew(2)->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD)
        {
            // Update displayed categories
            Categories_ScrollRight();
            Games_UpdateIndexes(Categories_GetDisplayed(2));
            Games_Refresh();
            // Draw categories normally
            UI_DrawCategories_Normal();
            // Draw games
            // UI_DrawGames();
            if ((!IsKeyDown(KEY_D) && !RB_HOLD) || currentMainMenuState == OPTIONS)
            {
                Var_SetScrollCateg(SCROLL_NO);
            }
            else
            {
                Game_New_Indexes();
                Games_NewRefresh();
                UI_ResetDisplayCoords_Scroll();
            }
            alphaCategories_Out = 1.0f;
            alphaCategories_In = 0.0f;
            UI_ResetDisplayCoords_Games();
            return;
        }
        // Update x coordinates for new games
        Games_GetNew(0)->x = Lerp(Games_GetNew(0)->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(1)->x = Lerp(Games_GetNew(1)->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(2)->x = Lerp(Games_GetNew(2)->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(3)->x = Lerp(Games_GetNew(3)->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        Games_GetNew(4)->x = Lerp(Games_GetNew(4)->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        // Update x coordinates for old games
        Games_GetDisplayed(1)->x = Lerp(Games_GetDisplayed(1)->x, LEFT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(2)->x = Lerp(Games_GetDisplayed(2)->x, LEFT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(3)->x = Lerp(Games_GetDisplayed(3)->x, CENTER_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(4)->x = Lerp(Games_GetDisplayed(4)->x, RIGHT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        Games_GetDisplayed(5)->x = Lerp(Games_GetDisplayed(5)->x, RIGHT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
    }
    // If scrolling to the left
    else
    {
        // If images close enough to final spot
        if (fabsf(Games_GetNew(2)->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD)
        {
            // Update displayed categories
            Categories_ScrollLeft();
            Games_UpdateIndexes(Categories_GetDisplayed(2));
            Games_Refresh();
            // Draw categories normally
            UI_DrawCategories_Normal();
            // Draw games
            // UI_DrawGames();
            if ((!IsKeyDown(KEY_A) && !LB_HOLD) || currentMainMenuState == OPTIONS)
            {
                Var_SetScrollCateg(SCROLL_NO);
            }
            else
            {
                Game_New_Indexes();
                Games_NewRefresh();
                UI_ResetDisplayCoords_Scroll();
            }
            alphaCategories_Out = 1.0f;
            alphaCategories_In = 0.0f;
            // Reset values for games
            UI_ResetDisplayCoords_Games();
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

// Function for centering an image on a certain X position
float UI_CenterImg_X(float width, float position)
{
    return (position - width / 2);
}

// Function for centering an image on a certain Y position
float UI_CenterImg_Y(float height, float position)
{
    return (position - height / 2.0f);
}

// Function to center text on a certain X position
float UI_CenterText_X(char *text, int fontSize, int position)
{
    float width = MeasureText(text, fontSize);
    return (position - (width / 2));
}






void MainMenu_DrawBase()
{
    // Clear background
    ClearBackground(BACKGROUND_CLR);

    Rectangle sourceRect = {0.0f, 0.0f, (float)currentBackground.width, (float)currentBackground.height};
    Rectangle destRect = {UI_CenterImg_X(SCREEN_W, CENTER_X), UI_CenterImg_Y(SCREEN_W / 3.0f * 2.0f, SCREEN_H / 2), SCREEN_W, SCREEN_W / 3.0f * 2.0f};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(currentBackground, sourceRect, destRect, origin, 0.0f, Fade(WHITE, 0.4));

    DrawRectangle(0, 0, SCREEN_W, START_SECTION_Y, Fade(Var_GetColor2(), 0.7));

    Vector2 startSection1 = {0, START_SECTION_Y};
    Vector2 endSection1 = {SCREEN_W, START_SECTION_Y};
    DrawLineEx(startSection1, endSection1, THICKNESS_LINE, Var_GetColor3());
    Vector2 startSection2 = {0, END_SECTION_Y};
    Vector2 endSection2 = {SCREEN_W, END_SECTION_Y};
    DrawLineEx(startSection2, endSection2, THICKNESS_LINE, Var_GetColor3());
}

ConsoleState UI_DrawOptions_Menu();

// Function for drawing the diagnostics screen
ConsoleState UI_DrawDiagnostics(ConsoleState currentConsoleState)
{
    UI_ChangeAlpha_Normal();
    // Clear background
    ClearBackground(BACKGROUND_CLR);
    Vector2 diagnosticsTitle_Size = MeasureTextEx(fontBold, DIAGNOSTICS_TITLE, DIAGNOSTICS_TITLE_SIZE, BUMPER_TXT_SPACE);
    Vector2 diagnosticsTitle_Text = {DIAGNOSTICS_TITLE_X, DIAGNOSTICS_TITLE_Y};
    DrawTextEx(fontBold, DIAGNOSTICS_TITLE, diagnosticsTitle_Text, DIAGNOSTICS_TITLE_SIZE, BUMPER_TXT_SPACE, Var_GetColor3());

    Vector2 diagnosticsResolution_Text = {RESOLUTION_TXT_X, RESOLUTION_TXT_Y};
    DrawTextEx(fontRegular, TextFormat("Resolution:  %d:%d", GetMonitorWidth(0), GetMonitorHeight(0)), diagnosticsResolution_Text, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());
    Vector2 diagnosticsFPS_Text = {FPS_TXT_X, FPS_TXT_Y};
    DrawTextEx(fontRegular, TextFormat("FPS:  %d", GetFPS()), diagnosticsFPS_Text, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, Var_GetColor3());

    // Transition
    switch (currentDiagState)
    {
    case DIAG_NORMAL:
        if (IsKeyPressed(KEY_TAB) || START_PRESS)
        {
            currentDiagState = DIAG_OPTIONS;
            currentOptionsState = LIST;
            displayBrightness = false;
            displayTheme = false;
        }
        break;

    case DIAG_OPTIONS:
        if (IsKeyPressed(KEY_TAB) || START_PRESS)
        {
            currentDiagState = DIAG_NORMAL;
        }
        break;
    }

    // Action
    switch (currentDiagState)
    {
    case DIAG_NORMAL:
        return STATE_VIEW_DIAG;
        break;

    case DIAG_OPTIONS:
        return UI_DrawOptions_Menu(currentConsoleState);
        break;
    }
}

void MainMenu_DrawDiagnostics()
{
    if (displayDiag)
    {
        Vector2 diagnosticsResolution_Text = {DIAG_RES_X, DIAG_RES_Y};
        DrawTextEx(fontRegular, TextFormat("Resolution:  %d:%d", GetMonitorWidth(0), GetMonitorHeight(0)), diagnosticsResolution_Text, DIAG_SIZE, DIAG_SPACING, Var_GetColor3());
        Vector2 diagnosticsFPS_Text = {DIAG_FPS_X, DIAG_FPS_Y};
        DrawTextEx(fontRegular, TextFormat("FPS:  %d", GetFPS()), diagnosticsFPS_Text, DIAG_SIZE, DIAG_SPACING, Var_GetColor3());
    }
}


void MainMenu_DrawBrightness()
{
    // Draw whole section
    Rectangle rect1 = {
        OPTIONS_RECT_X,
        OPTIONS_RECT_Y,
        OPTIONS_RECT_W,
        OPTIONS_RECT_H};
    DrawRectangleRoundedLinesEx(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawRectangleRounded(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, Fade(Var_GetColor2(), 1.0f));
    // Draw title
    Vector2 brightnessTxt_Size = MeasureTextEx(fontBold, BRIGHTNESS_TXT, BRIGHTNESS_TXT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 brightnessTxt = {BRIGHTNESS_TXT_X, BRIGHTNESS_TXT_Y};
    DrawTextEx(fontBold, BRIGHTNESS_TXT, brightnessTxt, BRIGHTNESS_TXT_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));

    DrawLineEx((Vector2){BRIGHTNESS_LINE_X_START, BRIGHTNESS_LINE_Y}, (Vector2){BRIGHTNESS_LINE_X_END, BRIGHTNESS_LINE_Y}, BRIGHTNESS_LINE_THICK, Var_GetColor1());

    // Transition
    switch (currentBrightPressed)
    {
    case NEITHER:
        if ((IsKeyDown(KEY_LEFT) || LS_LEFT) && Var_GetBright() < MIN_BRIGHTNESS)
        {
            currentBrightPressed = LEFT_PRESS;
            debugTimer = 0.0f;
        }
        else if ((IsKeyDown(KEY_RIGHT) || LS_RIGHT) && Var_GetBright() > MAX_BRIGHTNESS)
        {
            currentBrightPressed = RIGHT_PRESS;
            debugTimer = 0.0f;
        }
        break;

    case LEFT_PRESS:
        if (!IsKeyDown(KEY_LEFT) && !LS_LEFT)
        {
            currentBrightPressed = NEITHER;
        }
        break;

    case RIGHT_PRESS:
        if (!IsKeyDown(KEY_RIGHT) && !LS_RIGHT)
        {
            currentBrightPressed = NEITHER;
        }
        break;
    }

    // Action
    switch (currentBrightPressed)
    {
    case NEITHER:
        if (Var_GetBright() < MAX_BRIGHTNESS)
        {
            Var_SetBright(MAX_BRIGHTNESS);
        }
        else if (Var_GetBright() > MIN_BRIGHTNESS)
        {
            Var_SetBright(MIN_BRIGHTNESS);
        }
        break;

    case LEFT_PRESS:
        debugTimer += GetFrameTime();
        Var_AddBright(BRIGHTNESS_INCREMENT);
        Var_AddBrightX(- BRIGHTNESS_CIRCLE_INCREMENT);
        if (Var_GetBright() >= MIN_BRIGHTNESS) {
            Var_SetBright(MIN_BRIGHTNESS);
            Var_SetBrightX(BRIGHTNESS_LINE_X_START);
        }
        Update_UI_Txt();
        break;

    case RIGHT_PRESS:
        debugTimer += GetFrameTime();
        Var_AddBright(- BRIGHTNESS_INCREMENT);
        Var_AddBrightX(BRIGHTNESS_CIRCLE_INCREMENT);
        if (Var_GetBright() <= MAX_BRIGHTNESS) {
            Var_SetBright(MAX_BRIGHTNESS);
            Var_SetBrightX(BRIGHTNESS_CIRCLE_X);
        }
        Update_UI_Txt();
        break;
    }

    DrawCircle(Var_GetBrightX(), BRIGHTNESS_LINE_Y, BRIGHTNESS_RADIUS, Var_GetColor1());

    DrawCircle(BRIGHT_LS_LEFT_X + (SCREEN_W / 700.0f), BRIGHT_LS_Y, BRIGHT_LS_RADIUS, Fade(Var_GetColor3(), alphaSelectTxt));
    Vector2 point1 = {BRIGHT_LS_LEFT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y};
    Vector2 point2 = {BRIGHT_LS_LEFT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y + BRIGHT_LS_TRIANGLE_SIZE / 2};
    Vector2 point3 = {BRIGHT_LS_LEFT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y - BRIGHT_LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point1, point2, point3, Fade(Var_GetColor2(), alphaSelectTxt));

    DrawCircle(BRIGHT_LS_RIGHT_X - (SCREEN_W / 700.0f), BRIGHT_LS_Y, BRIGHT_LS_RADIUS, Fade(Var_GetColor3(), alphaSelectTxt));
    Vector2 point4 = {BRIGHT_LS_RIGHT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y};
    Vector2 point5 = {BRIGHT_LS_RIGHT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y + BRIGHT_LS_TRIANGLE_SIZE / 2};
    Vector2 point6 = {BRIGHT_LS_RIGHT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y - BRIGHT_LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point6, point5, point4, Fade(Var_GetColor2(), alphaSelectTxt));

    int brightPercent = (int)((MIN_BRIGHTNESS - Var_GetBright()) / (MIN_BRIGHTNESS / 100.0f));
    snprintf(brightnessPercent_Txt, sizeof(brightnessPercent_Txt), "%d%%", brightPercent);
    Vector2 brightnessPercent_Size = MeasureTextEx(fontBold, brightnessPercent_Txt, BRIGHTNESS_PERCENT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 brightnessPercent = {BRIGHTNESS_PERCENT_X, BRIGHTNESS_PERCENT_Y};
    DrawTextEx(fontBold, brightnessPercent_Txt, brightnessPercent, BRIGHTNESS_PERCENT_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
}

void MainMenu_DrawTheme()
{
    // Draw whole section
    Rectangle rect1 = {
        OPTIONS_RECT_X,
        OPTIONS_RECT_Y,
        OPTIONS_RECT_W,
        OPTIONS_RECT_H};
    DrawRectangleRoundedLinesEx(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawRectangleRounded(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, Fade(Var_GetColor2(), 1.0f));
    // Draw title
    Vector2 themeTxt_Size = MeasureTextEx(fontBold, THEME_TXT, THEME_TXT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 themeTxt = {THEME_TXT_X, THEME_TXT_Y};
    DrawTextEx(fontBold, THEME_TXT, themeTxt, THEME_TXT_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));

    Rectangle rect2 = {
        THEME_RECT_X,
        THEME_RECT_1_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLUE);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect2, THICKNESS_LINE, Var_GetColor3());

    Rectangle rect3 = {
        THEME_RECT_X,
        THEME_RECT_2_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, RED);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect3, THICKNESS_LINE, Var_GetColor3());

    Rectangle rect4 = {
        THEME_RECT_X,
        THEME_RECT_3_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, GREEN);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect4, THICKNESS_LINE, Var_GetColor3());

    Rectangle rect5 = {
        THEME_RECT_X,
        THEME_RECT_4_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, YELLOW);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect5, THICKNESS_LINE, Var_GetColor3());

    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_HOLD)
    {
        //mouseWasPressed = false;
    }

    // Transition
    switch (currentTheme)
    {
    case THEME_1:
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= 0.2f))
        {
            currentTheme = THEME_2;
            themeTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && themeTimeElapsed >= 0.2f))
        {
            currentTheme = THEME_4;
            themeTimeElapsed = 0.0f;
        }
        break;

    case THEME_2:
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= 0.2f))
        {
            currentTheme = THEME_3;
            themeTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && themeTimeElapsed >= 0.2f))
        {
            currentTheme = THEME_1;
            themeTimeElapsed = 0.0f;
        }
        break;

    case THEME_3:
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= 0.2f))
        {
            currentTheme = THEME_4;
            themeTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && themeTimeElapsed >= 0.2f))
        {
            currentTheme = THEME_2;
            themeTimeElapsed = 0.0f;
        }
        break;

    case THEME_4:
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && themeTimeElapsed >= 0.2f))
        {
            currentTheme = THEME_1;
            themeTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && themeTimeElapsed >= 0.2f))
        {
            currentTheme = THEME_3;
            themeTimeElapsed = 0.0f;
        }
        break;
    }

    // Action
    switch (currentTheme)
    {
    case THEME_1:
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_HOLD) {
            mouseLeftWasPressed = false;
        }
        themeTimeElapsed += GetFrameTime();
        DrawRectangleLinesEx(rect2, 2 * THICKNESS_LINE, Var_GetColor1());
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && mouseLeftWasPressed == false)
        {
            Var_SetColor1(BLUE);
            Var_SetColor2(BLACK);
            Var_SetColor3(WHITE);
            currentBackground = background_Blue;
            Update_UI_Txt();
        }
        break;

    case THEME_2:
        themeTimeElapsed += GetFrameTime();
        DrawRectangleLinesEx(rect3, 2 * THICKNESS_LINE, Var_GetColor1());
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            Var_SetColor1(RED);
            Var_SetColor2(BLACK);
            Var_SetColor3(WHITE);
            currentBackground = background_Red;
            Update_UI_Txt();
        }
        break;

    case THEME_3:
        themeTimeElapsed += GetFrameTime();
        DrawRectangleLinesEx(rect4, 2 * THICKNESS_LINE, Var_GetColor1());
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            Var_SetColor1(GREEN);
            Var_SetColor2(BLACK);
            Var_SetColor3(WHITE);
            currentBackground = background_Green;
            Update_UI_Txt();
        }
        break;

    case THEME_4:
        themeTimeElapsed += GetFrameTime();
        DrawRectangleLinesEx(rect5, 2 * THICKNESS_LINE, Var_GetColor1());
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            Var_SetColor1(YELLOW);
            Var_SetColor2(BLACK);
            Var_SetColor3(WHITE);
            currentBackground = background_Yellow;
        }
        break;
    }
}

void GameRefresh_DrawPaused() {
        // Draw whole section
    Rectangle rect1 = {
        OPTIONS_RECT_X,
        OPTIONS_RECT_Y,
        OPTIONS_RECT_W,
        OPTIONS_RECT_H};
    DrawRectangleRoundedLinesEx(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawRectangleRounded(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, Fade(Var_GetColor2(), 1.0f));
    // Draw title
    Vector2 pauseTitle_Size = MeasureTextEx(fontBold, PAUSE_TITLE, PAUSE_TITLE_SIZE, PAUSE_TITLE_SPACE);
    Vector2 pauseTitle = {PAUSE_TITLE_X, PAUSE_TITLE_Y};
    DrawTextEx(fontBold, PAUSE_TITLE, pauseTitle, PAUSE_TITLE_SIZE, PAUSE_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw view list
    Vector2 pauseRest_Size = MeasureTextEx(fontRegular, PAUSE_REST, PAUSE_REST_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 pauseRest = {PAUSE_REST_X, PAUSE_REST_Y};
    Rectangle rectRest = {
        OPTIONS_SELECT_RECT_X,
        PAUSE_REST_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        PAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectRest, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawTextEx(fontRegular, PAUSE_REST, pauseRest, PAUSE_REST_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw brightness section
    Vector2 pauseBrightness_Size = MeasureTextEx(fontRegular, PAUSE_BRIGHTNESS, PAUSE_BRIGHTNESS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 pauseBrightness = {PAUSE_BRIGHTNESS_X, PAUSE_BRIGHTNESS_Y};
    Rectangle rectBright = {
        OPTIONS_SELECT_RECT_X,
        PAUSE_BRIGHTNESS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        PAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectBright, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawTextEx(fontRegular, PAUSE_BRIGHTNESS, pauseBrightness, PAUSE_BRIGHTNESS_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw theme section
    Vector2 pauseControls_Size = MeasureTextEx(fontRegular, PAUSE_CONTROLS, PAUSE_CONTROLS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 pauseControls = {PAUSE_CONTROLS_X, PAUSE_CONTROLS_Y};
    Rectangle rectControls = {
        OPTIONS_SELECT_RECT_X,
        PAUSE_CONTROLS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        PAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectControls, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawTextEx(fontRegular, PAUSE_CONTROLS, pauseControls, PAUSE_CONTROLS_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw display diagnostics section
    Vector2 pauseDisplayDiagnostics_Size = MeasureTextEx(fontRegular, PAUSE_DISPLAY_DIAGNOSTICS, PAUSE_DISPLAY_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 pauseDisplayDiagnostics = {PAUSE_DISPLAY_DIAGNOSTICS_X, PAUSE_DISPLAY_DIAGNOSTICS_Y};
    Rectangle rectDDiag = {
        OPTIONS_SELECT_RECT_X,
        PAUSE_DISPLAY_DIAGNOSTICS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        PAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    char *diagTxt;
    if (displayDiag)
    {
        diagTxt = PAUSE_HIDE_DIAGNOSTICS;
    }
    else
    {
        diagTxt = PAUSE_DISPLAY_DIAGNOSTICS;
    }
    DrawTextEx(fontRegular, diagTxt, pauseDisplayDiagnostics, PAUSE_DISPLAY_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw exit section
    Vector2 pauseExit_Size = MeasureTextEx(fontRegular, PAUSE_EXIT, PAUSE_EXIT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 pauseExit = {PAUSE_EXIT_X, PAUSE_EXIT_Y};
    Rectangle rectExit = {
        OPTIONS_SELECT_RECT_X,
        PAUSE_EXIT_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        PAUSE_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectExit, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawTextEx(fontRegular, PAUSE_EXIT, pauseExit, PAUSE_EXIT_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
   
    // Draw select text
    Vector2 pauseSelect_Size = MeasureTextEx(fontRegular, PAUSE_SELECT, PAUSE_SELECT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 pauseSelect = {PAUSE_SELECT_X, PAUSE_SELECT_Y};
    DrawTextEx(fontRegular, PAUSE_SELECT, pauseSelect, PAUSE_SELECT_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw back text
    Vector2 pauseBack_Size = MeasureTextEx(fontRegular, PAUSE_BACK, PAUSE_BACK_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 pauseBack = {PAUSE_BACK_X, PAUSE_BACK_Y};
    DrawTextEx(fontRegular, PAUSE_BACK, pauseBack, PAUSE_BACK_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));

    if (currentPausedState == RESTART)
    {
        DrawRectangleRoundedLinesEx(rectRest, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
    else if (currentPausedState == PAUSED_BRIGHTNESS)
    {
        DrawRectangleRoundedLinesEx(rectBright, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
    else if (currentPausedState == PAUSED_CONTROLS)
    {
        DrawRectangleRoundedLinesEx(rectControls, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
    else if (currentPausedState == PAUSED_DIAGNOSTICS)
    {
        DrawRectangleRoundedLinesEx(rectDDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
    else
    {
        DrawRectangleRoundedLinesEx(rectExit, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
}


// Draw the options menu
void MainMenu_DrawOptions(optionsState currentOptionsState, ConsoleState currentConsoleState)
{
    // Draw whole section
    Rectangle rect1 = {
        OPTIONS_RECT_X,
        OPTIONS_RECT_Y,
        OPTIONS_RECT_W,
        OPTIONS_RECT_H};
    DrawRectangleRoundedLinesEx(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawRectangleRounded(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, Fade(Var_GetColor2(), 1.0f));
    // Draw title
    Vector2 optionsTitle_Size = MeasureTextEx(fontBold, OPTIONS_TITLE, OPTIONS_TITLE_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsTitle = {OPTIONS_TITLE_X, OPTIONS_TITLE_Y};
    DrawTextEx(fontBold, OPTIONS_TITLE, optionsTitle, OPTIONS_TITLE_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw view list
    Vector2 optionsList_Size = MeasureTextEx(fontRegular, OPTIONS_VIEW_LIST, OPTIONS_VIEW_LIST_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsList = {OPTIONS_VIEW_LIST_X, OPTIONS_VIEW_LIST_Y};
    Rectangle rectList = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_VIEW_LIST_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectList, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawTextEx(fontRegular, OPTIONS_VIEW_LIST, optionsList, OPTIONS_VIEW_LIST_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw brightness section
    Vector2 optionsBrightness_Size = MeasureTextEx(fontRegular, OPTIONS_BRIGHTNESS, OPTIONS_BRIGHTNESS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsBrightness = {OPTIONS_BRIGHTNESS_X, OPTIONS_BRIGHTNESS_Y};
    Rectangle rectBright = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_BRIGHTNESS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectBright, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawTextEx(fontRegular, OPTIONS_BRIGHTNESS, optionsBrightness, OPTIONS_BRIGHTNESS_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw theme section
    Vector2 optionsTheme_Size = MeasureTextEx(fontRegular, OPTIONS_THEME, OPTIONS_THEME_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsTheme = {OPTIONS_THEME_X, OPTIONS_THEME_Y};
    Rectangle rectTheme = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_THEME_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectTheme, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    DrawTextEx(fontRegular, OPTIONS_THEME, optionsTheme, OPTIONS_THEME_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw display diagnostics section
    Vector2 optionsDisplayDiagnostics_Size = MeasureTextEx(fontRegular, OPTIONS_DISPLAY_DIAGNOSTICS, OPTIONS_DISPLAY_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsDisplayDiagnostics = {OPTIONS_DISPLAY_DIAGNOSTICS_X, OPTIONS_DISPLAY_DIAGNOSTICS_Y};
    Rectangle rectDDiag = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_DISPLAY_DIAGNOSTICS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    char *diagTxt;
    if (displayDiag)
    {
        diagTxt = OPTIONS_HIDE_DIAGNOSTICS;
    }
    else
    {
        diagTxt = OPTIONS_DISPLAY_DIAGNOSTICS;
    }
    DrawTextEx(fontRegular, diagTxt, optionsDisplayDiagnostics, OPTIONS_DISPLAY_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw view diagnostics section
    Vector2 optionsDiagnostics_Size = MeasureTextEx(fontRegular, OPTIONS_DIAGNOSTICS, OPTIONS_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsDiagnostics = {OPTIONS_DIAGNOSTICS_X, OPTIONS_DIAGNOSTICS_Y};
    Rectangle rectDiag = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_DIAGNOSTICS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(Var_GetColor3(), 1.0f));
    char *menuTxt;
    if (currentConsoleState == STATE_VIEW_DIAG)
    {
        menuTxt = OPTIONS_MAIN;
    }
    else
    {
        menuTxt = OPTIONS_DIAGNOSTICS;
    }
    DrawTextEx(fontRegular, menuTxt, optionsDiagnostics, OPTIONS_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw select text
    Vector2 optionsSelect_Size = MeasureTextEx(fontRegular, OPTIONS_SELECT, OPTIONS_SELECT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsSelect = {OPTIONS_SELECT_X, OPTIONS_SELECT_Y};
    DrawTextEx(fontRegular, OPTIONS_SELECT, optionsSelect, OPTIONS_SELECT_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));
    // Draw back text
    Vector2 optionsBack_Size = MeasureTextEx(fontRegular, OPTIONS_BACK, OPTIONS_BACK_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsBack = {OPTIONS_BACK_X, OPTIONS_BACK_Y};
    DrawTextEx(fontRegular, OPTIONS_BACK, optionsBack, OPTIONS_BACK_SIZE, OPTIONS_TITLE_SPACE, Fade(Var_GetColor3(), 1.0f));

    if (currentOptionsState == LIST)
    {
        DrawRectangleRoundedLinesEx(rectList, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
    else if (currentOptionsState == BRIGHTNESS)
    {
        DrawRectangleRoundedLinesEx(rectBright, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
    else if (currentOptionsState == THEME)
    {
        DrawRectangleRoundedLinesEx(rectTheme, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
    else if (currentOptionsState == DISPLAY_DIAGNOSTICS)
    {
        DrawRectangleRoundedLinesEx(rectDDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
    else
    {
        DrawRectangleRoundedLinesEx(rectDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(Var_GetColor1(), 1.0f));
    }
}


// Tick function for options menu
ConsoleState UI_DrawOptions_Menu(ConsoleState currentConsoleState)
{
    // Transition
    switch (currentOptionsState)
    {
    case LIST:
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && optionsTimeElapsed >= 0.2f))
        {
            currentOptionsState = BRIGHTNESS;
            optionsTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && optionsTimeElapsed >= 0.2f))
        {
            currentOptionsState = VIEW_DIAGNOSTICS;
            optionsTimeElapsed = 0.0f;
        }
        break;

    case BRIGHTNESS:
        if (((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && optionsTimeElapsed >= 0.2f)) && displayBrightness == false)
        {
            currentOptionsState = THEME;
            optionsTimeElapsed = 0.0f;
        }
        else if (((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && optionsTimeElapsed >= 0.2f)) && displayBrightness == false)
        {
            currentOptionsState = LIST;
            optionsTimeElapsed = 0.0f;
        }
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && displayBrightness == false)
        {
            displayBrightness = true;
            currentBrightPressed = NEITHER;
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS)
        {
            displayBrightness = false;
        }
        break;

    case THEME:
        if (((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && optionsTimeElapsed >= 0.2f)) && displayTheme == false)
        {
            currentOptionsState = DISPLAY_DIAGNOSTICS;
            optionsTimeElapsed = 0.0f;
        }
        else if (((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && optionsTimeElapsed >= 0.2f)) && displayTheme == false)
        {
            currentOptionsState = BRIGHTNESS;
            optionsTimeElapsed = 0.0f;
        }
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && displayTheme == false)
        {
            displayTheme = true;
            currentTheme = THEME_1;
            mouseLeftWasPressed = true;
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS)
        {
            displayTheme = false;
        }
        break;

    case DISPLAY_DIAGNOSTICS:
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && optionsTimeElapsed >= 0.2f))
        {
            currentOptionsState = VIEW_DIAGNOSTICS;
            optionsTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && optionsTimeElapsed >= 0.2f))
        {
            currentOptionsState = THEME;
            optionsTimeElapsed = 0.0f;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            displayDiag = !displayDiag;
            Update_UI_Txt();
        }
        break;

    case VIEW_DIAGNOSTICS:
        if ((IsKeyPressed(KEY_DOWN)) || ((IsKeyPressed(KEY_DOWN) || LS_DOWN) && optionsTimeElapsed >= 0.2f))
        {
            currentOptionsState = LIST;
            optionsTimeElapsed = 0.0f;
        }
        else if ((IsKeyPressed(KEY_UP)) || ((IsKeyPressed(KEY_UP) || LS_UP) && optionsTimeElapsed >= 0.2f))
        {
            currentOptionsState = DISPLAY_DIAGNOSTICS;
            optionsTimeElapsed = 0.0f;
        }
        break;
    }

    // Action
    switch (currentOptionsState)
    {
    case LIST:
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_HOLD)
        {
            // mouseWasPressed = false;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            /// mouseWasPressed = true;
            return STATE_LIST;
        }
        MainMenu_DrawOptions(currentOptionsState, currentConsoleState);
        break;

    case BRIGHTNESS:
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_HOLD)
        {
            // mouseWasPressed = false;
        }
        if (displayBrightness == true)
        {
            MainMenu_DrawBrightness();
        }
        else
        {
            MainMenu_DrawOptions(currentOptionsState, currentConsoleState);
        }
        break;

    case THEME:
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_HOLD)
        {
            // mouseWasPressed = false;
        }
        if (displayTheme == true)
        {
            MainMenu_DrawTheme();
        }
        else
        {
            MainMenu_DrawOptions(currentOptionsState, currentConsoleState);
        }
        break;

    case DISPLAY_DIAGNOSTICS:
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_HOLD)
        {
            // mouseWasPressed = false;
        }
        MainMenu_DrawOptions(currentOptionsState, currentConsoleState);
        break;

    case VIEW_DIAGNOSTICS:
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !A_HOLD)
        {
            // mouseWasPressed = false;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            // mouseWasPressed = true;
            if (currentConsoleState == STATE_VIEW_DIAG)
            {
                currentMainMenuState = NORMAL;
                return STATE_MAIN_MENU;
            }
            else
            {
                currentDiagState = DIAG_NORMAL;
                return STATE_VIEW_DIAG;
            }
        }
        MainMenu_DrawOptions(currentOptionsState, currentConsoleState);
        break;
    }

    optionsTimeElapsed += GetFrameTime();
    if (optionsTimeElapsed > 1000000)
    {
        optionsTimeElapsed = 0.25f;
    }
    return currentConsoleState;
}

// Tick function for the main menu
ConsoleState MainMenu_Tick(ConsoleState currentConsoleState)
{

    MainMenu_DrawBase();

    // Transition
    switch (currentMainMenuState)
    {
    case NORMAL:
        // If not scrolling games to left, not scrolling categories, and the right input is pressed
        if (Var_GetScrollGames() != SCROLL_LEFT && Var_GetScrollCateg() == SCROLL_NO && (IsKeyDown(KEY_RIGHT) || LS_RIGHT))
        {
            // Scroll games to the right
            Var_SetScrollGames(SCROLL_RIGHT);
            currentMainMenuState = SCROLL_GAMES;
        }
        // If not scrolling games to right, not scrolling categories, and the right input is pressed
        else if (Var_GetScrollGames() != SCROLL_RIGHT && Var_GetScrollCateg() == SCROLL_NO && (IsKeyDown(KEY_LEFT) || LS_LEFT))
        {
            // Scroll games to the left
            Var_SetScrollGames(SCROLL_LEFT);
            currentMainMenuState = SCROLL_GAMES;
        }
        // If not scrolling categories to left and right input is pressed
        if (Var_GetScrollCateg() != SCROLL_LEFT && (IsKeyDown(KEY_D) || RB_HOLD))
        {
            // If not already scrolling cateogries
            if (Var_GetScrollCateg() == SCROLL_NO)
            {
                // Scroll cateogries to right and don't scroll games
                Var_SetScrollCateg(SCROLL_RIGHT);
                Var_SetScrollGames(SCROLL_NO);
                currentMainMenuState = SCROLL_CATEGORIES;
                // Get new games ready and reset the coordinates
                Game_New_Indexes();
                Games_NewRefresh();
                UI_ResetDisplayCoords_Scroll();
            }
        }
        // If not scrolling categories to right and right input is pressed
        else if (Var_GetScrollCateg() != SCROLL_RIGHT && (IsKeyDown(KEY_A) || LB_HOLD))
        {
            // If not already scrolling cateogries
            if (Var_GetScrollCateg() == SCROLL_NO)
            {
                // Scroll cateogries to right and don't scroll games
                Var_SetScrollCateg(SCROLL_LEFT);
                Var_SetScrollGames(SCROLL_NO);
                currentMainMenuState = SCROLL_CATEGORIES;
                // Get new games ready and reset the coordinates
                Game_New_Indexes();
                Games_NewRefresh();
                UI_ResetDisplayCoords_Scroll();
            }
        }
        if (IsKeyPressed(KEY_TAB) || START_PRESS)
        {
            currentMainMenuState = OPTIONS;
            currentOptionsState = LIST;
            displayBrightness = false;
            displayTheme = false;
        }
        break;

    case SCROLL_GAMES:
        if (Var_GetScrollGames() == SCROLL_NO)
        {
            currentMainMenuState = NORMAL;
            alphaSelectTxt_Blink = true;
            alphaSelectTxt_TimeElapsed = 0.0f;
        }
        if (IsKeyPressed(KEY_TAB) || START_PRESS)
        {
            currentMainMenuState = OPTIONS;
            currentOptionsState = LIST;
            displayBrightness = false;
            displayTheme = false;
        }
        if (IsKeyPressed(KEY_A) || LB_PRESS)
        {
            Var_SetScrollGames(SCROLL_NO);
            // Update displayed games
            Games_ScrollLeft();
            // Reset game values
            UI_ResetDisplayCoords_Games();
            Var_SetScrollCateg(SCROLL_LEFT);
            currentMainMenuState = SCROLL_CATEGORIES;
            Game_New_Indexes();
            Games_NewRefresh();
            UI_ResetDisplayCoords_Scroll();
        }
        else if (IsKeyPressed(KEY_D) || RB_PRESS)
        {
            Var_SetScrollGames(SCROLL_NO);
            // Update displayed games
            Games_ScrollRight();
            // Reset game values
            UI_ResetDisplayCoords_Games();
            Var_SetScrollCateg(SCROLL_RIGHT);
            currentMainMenuState = SCROLL_CATEGORIES;
            Game_New_Indexes();
            Games_NewRefresh();
            UI_ResetDisplayCoords_Scroll();
        }
        break;

    case SCROLL_CATEGORIES:
        if (Var_GetScrollCateg() == SCROLL_NO)
        {
            currentMainMenuState = NORMAL;
            alphaSelectTxt_Blink = true;
            alphaSelectTxt_TimeElapsed = 0.0f;
        }
        if (IsKeyPressed(KEY_TAB) || START_PRESS)
        {
            currentMainMenuState = OPTIONS;
            currentOptionsState = LIST;
            displayBrightness = false;
            displayTheme = false;
        }
        break;

    case OPTIONS:
        if (IsKeyPressed(KEY_TAB) || START_PRESS || ((IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || B_PRESS) && displayBrightness == false && displayTheme == false))
        {
            currentMainMenuState = NORMAL;
        }
        break;
    }

    // Action
    switch (currentMainMenuState)
    {
    case NORMAL:
        UI_ChangeAlpha_Normal();
        UI_DrawCategories_Normal();
        MainMenu_DrawGames_Normal();
        // Draw bumpers
        UI_DrawBumpers();
        // Draw bottom section of screen
        UI_DrawBottom();
        MainMenu_DrawDiagnostics();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            return STATE_APP_LAUNCHER;
        }
        return STATE_MAIN_MENU;
        break;

    case SCROLL_GAMES:
        UI_ChangeAlpha_ScrollGames();
        UI_DrawCategories_Normal();
        UI_DrawScroll_Games();
        // Draw outline for selected game
        DrawRectangleLinesEx(
            (Rectangle){
                img_X - THICKNESS_SELECT_GAME,
                img_Y - THICKNESS_SELECT_GAME,
                img_W + (2 * THICKNESS_SELECT_GAME),
                img_H + (2 * THICKNESS_SELECT_GAME)},
            THICKNESS_SELECT_GAME,
            Fade(Var_GetColor1(), alphaGames));
        // Draw bumpers
        UI_DrawBumpers();
        // Draw bottom section of screen
        UI_DrawBottom();
        MainMenu_DrawDiagnostics();
        return STATE_MAIN_MENU;
        break;

    case SCROLL_CATEGORIES:
        UI_ChangeAlpha_ScrollCategories();
        UI_DrawScroll_Categories();
        MainMenu_DrawGames_Normal();
        // Draw bumpers
        UI_DrawBumpers();
        // Draw bottom section of screen
        UI_DrawBottom();
        MainMenu_DrawDiagnostics();
        return STATE_MAIN_MENU;
        break;

    case OPTIONS:
        if (Var_GetScrollGames() != SCROLL_NO)
        {
            UI_ChangeAlpha_ScrollGames();
            UI_DrawCategories_Normal();
            UI_DrawScroll_Games();
            // Draw outline for selected game
            DrawRectangleLinesEx(
                (Rectangle){
                    img_X - THICKNESS_SELECT_GAME,
                    img_Y - THICKNESS_SELECT_GAME,
                    img_W + (2 * THICKNESS_SELECT_GAME),
                    img_H + (2 * THICKNESS_SELECT_GAME)},
                THICKNESS_SELECT_GAME,
                Fade(Var_GetColor1(), alphaGames));
        }
        else if (Var_GetScrollCateg() != SCROLL_NO)
        {
            UI_ChangeAlpha_ScrollCategories();
            UI_DrawScroll_Categories();
            MainMenu_DrawGames_Normal();
        }
        else
        {
            UI_ChangeAlpha_Normal();
            UI_DrawCategories_Normal();
            MainMenu_DrawGames_Normal();
        }
        // Draw bumpers
        UI_DrawBumpers();
        // Draw bottom section of screen
        UI_DrawBottom();
        MainMenu_DrawDiagnostics();
        return UI_DrawOptions_Menu(currentConsoleState);
        break;
    }
}


void UI_Init() {
    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);
    //Load the logo textures
    spiderLogo = LoadTexture("/home/tywebb1724/Desktop/Gaming-Console/assets/covers/logo/LogoBlack.png");
    background_Blue = LoadTexture("./assets/covers/logo/BlueBackground.png");
    background_Red = LoadTexture("./assets/covers/logo/RedBackground.png");
    background_Green = LoadTexture("./assets/covers/logo/GreenBackground.png");
    background_Yellow = LoadTexture("./assets/covers/logo/YellowBackground.png");
    //Initialize the alphas
    alphaCategories_Out = 1.0f;
    alphaCategories_In = 0.0f;

    currentTheme = THEME_1;

    //Reset the coordinates for the games
    UI_ResetDisplayCoords_Games();
    float tempLen1;
    float tempLen2;
    for (int i = 0; i < GAMES_LEN; i++) {
        (*Games_Get(i)).cover.id = 0;
        tempLen1 = MeasureTextEx(fontBold, (*Games_Get(i)).title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x;
        tempLen2 = MeasureTextEx(fontBold, (*Games_Get(i)).console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x;
        if (tempLen1 > maxLen) {
            maxLen = tempLen1;
        }
        if (tempLen2 > maxLen) {
            maxLen = tempLen2;
        }
    }
    char color1[10] = "", color2[10] = "", color3[10] = "", bright[32] = "", circX[32] = "", diag[5] = "";
    FILE* f = fopen("/home/tywebb1724/Desktop/Gaming-Console/assets/system/ui.txt", "r");
    if (f) {

        if (fgets(color1, sizeof(color1), f)) {
            color1[strcspn(color1, "\n")] = '\0';
            Var_SetColor1(NametoColor(color1));
            currentBackground = NametoBackground(color1);
        }
        else {
            Var_SetColor1(BLUE);
            currentBackground = background_Blue;
        }
        if (fgets(color2, sizeof(color2), f)) {
            color2[strcspn(color2, "\n")] = '\0';
            Var_SetColor2(NametoColor(color2));;
        }
        else {
            Var_SetColor2(BLACK);
        }
        if (fgets(color3, sizeof(color3), f)) {
            color3[strcspn(color3, "\n")] = '\0';
            Var_SetColor3(NametoColor(color3));;
        }
        else {
            Var_SetColor3(WHITE);
        }
        if (fgets(bright, sizeof(bright), f)) {
            bright[strcspn(bright, "\n")] = '\0';
            Var_SetBright(atof(bright));

            if (fgets(circX, sizeof(circX), f)) { 
                circX[strcspn(circX, "\n")] = '\0';
                Var_SetBrightX(atof(circX));
            }
            else {
                Var_SetBright(MAX_BRIGHTNESS);
                Var_SetBrightX(BRIGHTNESS_CIRCLE_X);
            }
        }
        else {
            Var_SetBright(MAX_BRIGHTNESS);
            Var_SetBrightX(BRIGHTNESS_CIRCLE_X);
        }
        if (fgets(diag, sizeof(diag), f)) {
            diag[strcspn(diag, "\n")] = '\0';
            displayDiag = atoi(diag);
        }
        else {
            displayDiag = false;
        }
        fclose(f);
    }
    maxLen = 0;
    for (int i = 0; i < GAMES_LEN; i++) {
        if (MeasureTextEx(fontBold, Games_Get(i)->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE) > maxLen) {
            maxLen = MeasureTextEx(fontBold, Games_Get(i)->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
        }
    }
    for (int i = 0; i < CATEGORIES_LEN; i++) {
        if (MeasureTextEx(fontBold, Categories_Get(i), BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE) > maxLen) {
            maxLen = MeasureTextEx(fontBold, Categories_Get(i)->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
        }
    }
    if (MeasureTextEx(fontBold, BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE) > maxLen) {
        maxLen = MeasureTextEx(fontBold, BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    }
}