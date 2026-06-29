#include "ui.h"
#include "raylib.h"
#include "raymath.h"
#include "games.h"
#include <string.h>
#include <math.h>
#include "states.h"
#include "categories.h"
#include "ui_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mainMenuState currentMainMenuState = NORMAL;
optionsState currentOptionsState = LIST;
screenState currentScreenState = OPTIONS;
diagState currentDiagState = DIAG_NORMAL;
float optionsTimeElapsed = 0.0f;
bool openDiagnostics = false;
bool displayBrightness = false;
bool displayTheme = false;
bool displayDiag;
float brightCircleX;
brightPressed currentBrightPressed;

float debugTimer = 0.0f;

Color themeColor1;
Color themeColor2;
Color themeColor3;

char brightnessPercent_Txt[5];

bool mouseLeftWasPressed = false;
bool mouseRightWasPressed = false;

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
// Variables to show whether user is scrolling
Scroll scrollGames;
Scroll scrollCategories;
float brightness;

theme_t currentTheme;
theme_t currentThemeSelected;
float themeTimeElapsed;

const char *ColorToName(Color c)
{
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
    // ... add the colors you actually use ...
    return "UNKNOWN"; // fallback if it matches nothing
}

void Update_UI_Txt()
{
    FILE *f = fopen("/home/tywebb1724/Desktop/Gaming-Console/assets/system/ui.txt", "w");
    if (f)
    {
        fprintf(f, "%s\n", ColorToName(themeColor1));
        fprintf(f, "%s\n", ColorToName(themeColor2));
        fprintf(f, "%s\n", ColorToName(themeColor3));
        fprintf(f, "%f\n", brightness);
        fprintf(f, "%f\n", brightCircleX);
        fprintf(f, "%d\n", displayDiag);
        fclose(f);
    }
}

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

// Reset the coordinates for the displayed games after games scroll
void UI_ResetDisplayCoords_Games()
{
    // Reset x coordinates
    gamesDisplayed[0]->x = LEFT3_GAME_X;
    gamesDisplayed[1]->x = LEFT2_GAME_X;
    gamesDisplayed[2]->x = LEFT1_GAME_X;
    gamesDisplayed[3]->x = CENTER_GAME_X;
    gamesDisplayed[4]->x = RIGHT1_GAME_X;
    gamesDisplayed[5]->x = RIGHT2_GAME_X;
    gamesDisplayed[6]->x = RIGHT3_GAME_X;
    // Reset y coordinates
    gamesDisplayed[0]->y = SIDE3_GAME_Y;
    gamesDisplayed[1]->y = SIDE2_GAME_Y;
    gamesDisplayed[2]->y = SIDE1_GAME_Y;
    gamesDisplayed[3]->y = CENTER_GAME_Y;
    gamesDisplayed[4]->y = SIDE1_GAME_Y;
    gamesDisplayed[5]->y = SIDE2_GAME_Y;
    gamesDisplayed[6]->y = SIDE3_GAME_Y;
    // Reset width values
    gamesDisplayed[0]->w = SIDE3_GAME_W;
    gamesDisplayed[1]->w = SIDE2_GAME_W;
    gamesDisplayed[2]->w = SIDE1_GAME_W;
    gamesDisplayed[3]->w = CENTER_GAME_W;
    gamesDisplayed[4]->w = SIDE1_GAME_W;
    gamesDisplayed[5]->w = SIDE2_GAME_W;
    gamesDisplayed[6]->w = SIDE3_GAME_W;
    // Reset height values
    gamesDisplayed[0]->h = SIDE3_GAME_H;
    gamesDisplayed[1]->h = SIDE2_GAME_H;
    gamesDisplayed[2]->h = SIDE1_GAME_H;
    gamesDisplayed[3]->h = CENTER_GAME_H;
    gamesDisplayed[4]->h = SIDE1_GAME_H;
    gamesDisplayed[5]->h = SIDE2_GAME_H;
    gamesDisplayed[6]->h = SIDE3_GAME_H;
}

// Reset the coordinates for the displayed games after category scroll
void UI_ResetDisplayCoords_Scroll()
{
    // If scrolling to the right
    if (scrollCategories == SCROLL_RIGHT)
    {
        // Reset x coordinates
        newGamesDisplayed[0]->x = LEFT2_SCROLLL_X;
        newGamesDisplayed[1]->x = LEFT1_SCROLLL_X;
        newGamesDisplayed[2]->x = CENTER_SCROLLL_X;
        newGamesDisplayed[3]->x = RIGHT1_SCROLLL_X;
        newGamesDisplayed[4]->x = RIGHT2_SCROLLL_X;
    }
    // If scrolling to the left
    else
    {
        // Reset x coordinates
        newGamesDisplayed[0]->x = LEFT2_SCROLLR_X;
        newGamesDisplayed[1]->x = LEFT1_SCROLLR_X;
        newGamesDisplayed[2]->x = CENTER_SCROLLR_X;
        newGamesDisplayed[3]->x = RIGHT1_SCROLLR_X;
        newGamesDisplayed[4]->x = RIGHT2_SCROLLR_X;
    }
    // Reset y coordinates
    newGamesDisplayed[0]->y = SIDE2_GAME_Y;
    newGamesDisplayed[1]->y = SIDE1_GAME_Y;
    newGamesDisplayed[2]->y = CENTER_GAME_Y;
    newGamesDisplayed[3]->y = SIDE1_GAME_Y;
    newGamesDisplayed[4]->y = SIDE2_GAME_Y;
    // Reset width values
    newGamesDisplayed[0]->w = SIDE2_GAME_W;
    newGamesDisplayed[1]->w = SIDE1_GAME_W;
    newGamesDisplayed[2]->w = CENTER_GAME_W;
    newGamesDisplayed[3]->w = SIDE1_GAME_W;
    newGamesDisplayed[4]->w = SIDE2_GAME_W;
    // Reset height values
    newGamesDisplayed[0]->h = SIDE2_GAME_H;
    newGamesDisplayed[1]->h = SIDE1_GAME_H;
    newGamesDisplayed[2]->h = CENTER_GAME_H;
    newGamesDisplayed[3]->h = SIDE1_GAME_H;
    newGamesDisplayed[4]->h = SIDE2_GAME_H;
}

// Function for drawing an image
void UI_DrawImage(float alpha)
{
    // If image loaded, draw image
    if (img.id > 0)
    {
        Rectangle sourceRect = {0.0f, 0.0f, (float)img.width, (float)img.height};
        Rectangle destRect = {(float)img_X, img_Y, img_W, img_H};
        Vector2 origin = {0.0f, 0.0f};
        DrawTexturePro(img, sourceRect, destRect, origin, 0.0f, Fade(WHITE, alpha));
    }
    // Draw gray placeholder if image isn't loaded
    else
    {
        DrawRectangle(img_X, img_Y, img_W, img_H, Fade(LIGHTGRAY, alpha));
        DrawRectangleLines(img_X, img_Y, img_W, img_H, Fade(GRAY, alpha));
    }
}

// Function for drawing a game cover
void UI_DrawGame(int i, float alpha, bool scroll)
{
    // If scrolling
    if (scroll)
    {
        // Update values of image to draw
        img_Y = newGamesDisplayed[i]->y;
        img_W = newGamesDisplayed[i]->w;
        img_H = newGamesDisplayed[i]->h;
        img_X = newGamesDisplayed[i]->x;
        img = newGamesDisplayed[i]->cover;
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
        img_Y = gamesDisplayed[i]->y;
        img_W = gamesDisplayed[i]->w;
        img_H = gamesDisplayed[i]->h;
        img_X = gamesDisplayed[i]->x;
        img = gamesDisplayed[i]->cover;
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
    if (scrollGames == SCROLL_RIGHT)
    {
        // Draw the games
        UI_DrawGames_Right();
        // If images close enough to final spot
        if (fabsf(gamesDisplayed[4]->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD)
        {
            if ((!IsKeyDown(KEY_RIGHT && !LS_RIGHT)) || currentMainMenuState == OPTIONS)
            {
                scrollGames = SCROLL_NO;
            }
            // Update displayed games
            Games_ScrollRight();
            // Reset game values
            UI_ResetDisplayCoords_Games();
            return;
        }
        // Update x coordinates
        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT3_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[6]->x = Lerp(gamesDisplayed[6]->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        // Update y coordinates
        gamesDisplayed[1]->y = Lerp(gamesDisplayed[1]->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->y = Lerp(gamesDisplayed[2]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->y = Lerp(gamesDisplayed[3]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->y = Lerp(gamesDisplayed[4]->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->y = Lerp(gamesDisplayed[5]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[6]->y = Lerp(gamesDisplayed[6]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        // Update width values
        gamesDisplayed[1]->w = Lerp(gamesDisplayed[1]->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->w = Lerp(gamesDisplayed[2]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->w = Lerp(gamesDisplayed[3]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->w = Lerp(gamesDisplayed[4]->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->w = Lerp(gamesDisplayed[5]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[6]->w = Lerp(gamesDisplayed[6]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        // Update height values
        gamesDisplayed[1]->h = Lerp(gamesDisplayed[1]->h, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->h = Lerp(gamesDisplayed[2]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->h = Lerp(gamesDisplayed[3]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->h = Lerp(gamesDisplayed[4]->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->h = Lerp(gamesDisplayed[5]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[6]->h = Lerp(gamesDisplayed[6]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
    }
    // If scrolling to the left
    else if (scrollGames == SCROLL_LEFT)
    {
        // Draw the games
        UI_DrawGames_Left();
        // If images close enough to final spot
        if (fabsf(gamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD)
        {
            if ((!IsKeyDown(KEY_LEFT) && !LS_LEFT) || currentMainMenuState == OPTIONS)
            {
                scrollGames = SCROLL_NO;
            }
            // Update displayed games
            Games_ScrollLeft();
            // Reset game values
            UI_ResetDisplayCoords_Games();
            return;
        }
        // Update x coordinates
        gamesDisplayed[0]->x = Lerp(gamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT3_GAME_X, SCROLL_GAMES_SPEED);
        // Update y coordinates
        gamesDisplayed[0]->y = Lerp(gamesDisplayed[0]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->y = Lerp(gamesDisplayed[1]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->y = Lerp(gamesDisplayed[2]->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->y = Lerp(gamesDisplayed[3]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->y = Lerp(gamesDisplayed[4]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->y = Lerp(gamesDisplayed[5]->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        // Update width values
        gamesDisplayed[0]->w = Lerp(gamesDisplayed[0]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->w = Lerp(gamesDisplayed[1]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->w = Lerp(gamesDisplayed[2]->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->w = Lerp(gamesDisplayed[3]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->w = Lerp(gamesDisplayed[4]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->w = Lerp(gamesDisplayed[5]->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        // Update height values
        gamesDisplayed[0]->h = Lerp(gamesDisplayed[0]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->h = Lerp(gamesDisplayed[1]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->h = Lerp(gamesDisplayed[2]->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->h = Lerp(gamesDisplayed[3]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->h = Lerp(gamesDisplayed[4]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->h = Lerp(gamesDisplayed[5]->h, SIDE3_GAME_H, SCROLL_GAMES_SPEED);
    }
}

void MainMenu_DrawGames_Normal()
{
    // If scrolling categories
    if (scrollCategories != SCROLL_NO)
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
        Fade(themeColor1, alphaGames));
}

// Function for drawing the cateogires when they are static
void UI_DrawCategories_Normal()
{
    // Draw center category
    Vector2 size = MeasureTextEx(fontBold, categoriesDisplayed[2].name, CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 centerCateg = {CENTER_CATEG_X, CENTER_CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[2].name, centerCateg, CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(themeColor3, alphaCategories_Out));

    // Draw left category
    size = MeasureTextEx(fontBold, categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 left1Categ = {LEFT1_CATEG_X, SIDE_CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[1].name, left1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(themeColor3, alphaCategories_Out));
    // Draw right category
    size = MeasureTextEx(fontBold, categoriesDisplayed[3].name, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 right1Categ = {RIGHT1_CATEG_X, SIDE_CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[3].name, right1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(themeColor3, alphaCategories_Out));
}

// Function for drawing the animation of scrolling categories
void UI_DrawScroll_Categories()
{
    // If scrolling to the right
    if (scrollCategories == SCROLL_RIGHT)
    {
        // If images close enough to final spot
        if (fabsf(newGamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD)
        {
            // Update displayed categories
            Categories_ScrollRight();
            // Draw categories normally
            UI_DrawCategories_Normal();
            // Draw games
            // UI_DrawGames();
            if ((!IsKeyDown(KEY_D) && !RB_HOLD) || currentMainMenuState == OPTIONS)
            {
                scrollCategories = SCROLL_NO;
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
        newGamesDisplayed[0]->x = Lerp(newGamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[1]->x = Lerp(newGamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[2]->x = Lerp(newGamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[3]->x = Lerp(newGamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[4]->x = Lerp(newGamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        // Update x coordinates for old games
        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, LEFT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, CENTER_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, RIGHT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
    }
    // If scrolling to the left
    else
    {
        // If images close enough to final spot
        if (fabsf(newGamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD)
        {
            // Update displayed categories
            Categories_ScrollLeft();
            // Draw categories normally
            UI_DrawCategories_Normal();
            // Draw games
            // UI_DrawGames();
            if ((!IsKeyDown(KEY_A) && !LB_HOLD) || currentMainMenuState == OPTIONS)
            {
                scrollCategories = SCROLL_NO;
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
        newGamesDisplayed[0]->x = Lerp(newGamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[1]->x = Lerp(newGamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[2]->x = Lerp(newGamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[3]->x = Lerp(newGamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[4]->x = Lerp(newGamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        // Update x coordinates for old games
        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT2_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, LEFT1_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, CENTER_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, RIGHT1_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT2_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
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

void UI_DrawOptionsBtn()
{
    // Draw options button
    DrawCircle(OPTION_BTN_X, OPTION_BTN_Y, LS_RADIUS, themeColor3);
    Vector2 start2 = {OPTION_LINE_START, OPTION_LINE2_Y};
    Vector2 end2 = {OPTION_LINE_END, OPTION_LINE2_Y};
    DrawLineEx(start2, end2, OPTION_THICKNESS, themeColor2);
    Vector2 start1 = {OPTION_LINE_START, OPTION_LINE1_Y};
    Vector2 end1 = {OPTION_LINE_END, OPTION_LINE1_Y};
    DrawLineEx(start1, end1, OPTION_THICKNESS2, themeColor2);
    Vector2 start3 = {OPTION_LINE_START, OPTION_LINE3_Y};
    Vector2 end3 = {OPTION_LINE_END, OPTION_LINE3_Y};
    DrawLineEx(start3, end3, OPTION_THICKNESS3, themeColor2);

    Vector2 options_Size = MeasureTextEx(fontBold, OPTIONS_TXT, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE);
    Vector2 options_Text = {OPTIONS_TXT_X, OPTIONS_TXT_Y};
    DrawTextEx(fontRegular, OPTIONS_TXT, options_Text, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE, themeColor3);
}

// Function to draw the bumpers
void UI_DrawBumpers()
{
    // Values for left bumper
    Rectangle bumperL = {
        BUMPER_L_X,
        BUMPER_Y,
        BUMPER_W,
        BUMPER_H};
    // Values for right bumper
    Rectangle bumperR = {
        BUMPER_R_X,
        BUMPER_Y,
        BUMPER_W,
        BUMPER_H};

    // Values for bumper text and drawing the text
    Vector2 bumperL_Size = MeasureTextEx(fontBold, BUMPER_L_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperR_Size = MeasureTextEx(fontBold, BUMPER_R_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperR_Text = {BUMPER_R_TXT_X, BUMPER_TXT_Y};
    Vector2 bumperL_Text = {BUMPER_L_TXT_X, BUMPER_TXT_Y};

    // Draw bumper
    if (scrollCategories == SCROLL_RIGHT)
    {
        DrawRectangleRounded(bumperL, BUMPER_ROUND, BUMPER_SEGMENTS, Fade(themeColor2, alphaCategories_Out));
        DrawRectangleRounded(bumperR, BUMPER_ROUND, BUMPER_SEGMENTS, themeColor3);
        DrawTextEx(fontBold, BUMPER_R_TXT, bumperR_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, themeColor2);
        DrawTextEx(fontBold, BUMPER_L_TXT, bumperL_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Fade(themeColor2, alphaCategories_Out));
    }
    else if (scrollCategories == SCROLL_LEFT)
    {
        DrawRectangleRounded(bumperL, BUMPER_ROUND, BUMPER_SEGMENTS, themeColor3);
        DrawRectangleRounded(bumperR, BUMPER_ROUND, BUMPER_SEGMENTS, Fade(themeColor2, alphaCategories_Out));
        DrawTextEx(fontBold, BUMPER_R_TXT, bumperR_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, Fade(themeColor2, alphaCategories_Out));
        DrawTextEx(fontBold, BUMPER_L_TXT, bumperL_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, themeColor2);
    }
    else
    {
        DrawRectangleRounded(bumperL, BUMPER_ROUND, BUMPER_SEGMENTS, themeColor3);
        DrawRectangleRounded(bumperR, BUMPER_ROUND, BUMPER_SEGMENTS, themeColor3);
        DrawTextEx(fontBold, BUMPER_R_TXT, bumperR_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, themeColor2);
        DrawTextEx(fontBold, BUMPER_L_TXT, bumperL_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, themeColor2);
    }

    UI_DrawOptionsBtn();

    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(SMALL_LOGO_SIZE, CENTER_X), UI_CenterImg_Y(SMALL_LOGO_SIZE, OPTION_BTN_Y + SCREEN_H / 125.0f), SMALL_LOGO_SIZE, SMALL_LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

// Function for drawing the select controls at the bottom
void UI_DrawBottom()
{

    DrawCircle(LS_LEFT_X + (SCREEN_W / 700.0f), LS_Y, LS_RADIUS, Fade(themeColor3, alphaSelectTxt));
    Vector2 point1 = {LS_LEFT_X - (LS_TRIANGLE_SIZE / 2), LS_Y};
    Vector2 point2 = {LS_LEFT_X + (LS_TRIANGLE_SIZE / 2), LS_Y + LS_TRIANGLE_SIZE / 2};
    Vector2 point3 = {LS_LEFT_X + (LS_TRIANGLE_SIZE / 2), LS_Y - LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point1, point2, point3, Fade(themeColor2, alphaSelectTxt));

    DrawCircle(LS_RIGHT_X - (SCREEN_W / 700.0f), LS_Y, LS_RADIUS, Fade(themeColor3, alphaSelectTxt));
    Vector2 point4 = {LS_RIGHT_X + (LS_TRIANGLE_SIZE / 2), LS_Y};
    Vector2 point5 = {LS_RIGHT_X - (LS_TRIANGLE_SIZE / 2), LS_Y + LS_TRIANGLE_SIZE / 2};
    Vector2 point6 = {LS_RIGHT_X - (LS_TRIANGLE_SIZE / 2), LS_Y - LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point6, point5, point4, Fade(themeColor2, alphaSelectTxt));

    // Values for Text
    Vector2 gameText_Size = MeasureTextEx(fontBold, gamesDisplayed[3]->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 bottomText_Size = MeasureTextEx(fontBold, BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 consoleText_Size = MeasureTextEx(fontBold, gamesDisplayed[3]->console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 gameText = {BOTTOM_TXT_X, GAME_TXT_Y};
    Vector2 bottomText = {BOTTOM_TXT_X, BOTTOM_TXT_Y};
    Vector2 consoleText = {BOTTOM_TXT_X, CONSOLE_TXT_Y};
    int rectWidth;
    if (gameText_Size.x > bottomText_Size.x && gameText_Size.x > consoleText_Size.x)
    {
        rectWidth = gameText_Size.x;
    }
    else if (bottomText_Size.x > gameText_Size.x && bottomText_Size.x > consoleText_Size.x)
    {
        rectWidth = bottomText_Size.x;
    }
    else
    {
        rectWidth = consoleText_Size.x;
    }
    // Draw section
    Rectangle rect = {
        BOTTOM_RECT_X,
        BOTTOM_RECT_Y,
        BOTTOM_RECT_W,
        BOTTOM_RECT_H};

    DrawRectangleRoundedLinesEx(rect, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, alphaSelectBox));
    DrawRectangleRounded(rect, BOTTOM_ROUND, BOTTOM_SEGMENTS, Fade(themeColor2, alphaSelectBox));
    // Draw text
    DrawTextEx(fontBold, gamesDisplayed[3]->title, gameText, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(themeColor3, alphaSelectTxt));
    DrawTextEx(fontBold, gamesDisplayed[3]->console, consoleText, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(themeColor3, alphaSelectTxt));
    DrawTextEx(fontBold, BOTTOM_TXT, bottomText, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(themeColor3, alphaSelectTxt));
}

// Function for drawing the boot screen
void UI_DrawBootScreen()
{
    // Clear background
    ClearBackground(BACKGROUND_CLR);
    // Draw logo
    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(LOGO_SIZE, CENTER_X), UI_CenterImg_Y(LOGO_SIZE, SCREEN_H / 2), LOGO_SIZE, LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

// Function for drawing the heading at the top of the screen
void UI_DrawHeading()
{
    // DrawText(COMPANY_NAME, UI_CenterText_X(COMPANY_NAME, COMPANY_TXT_SIZE, CENTER_X), COMPANY_Y, COMPANY_TXT_SIZE, themeColor1);
    // DrawText(PICK_GAME_TXT, UI_CenterText_X(PICK_GAME_TXT, PICK_GAME_TXT_SIZE, CENTER_X), PICK_GAME_Y, PICK_GAME_TXT_SIZE, themeColor1);
    // DrawRectangle(0, 0, SCREEN_W, TOP_Y, themeColor3);
    // DrawLine(0, TOP_Y, SCREEN_W, TOP_Y, themeColor1);
}

void MainMenu_DrawBase()
{
    // Clear background
    ClearBackground(BACKGROUND_CLR);

    Rectangle sourceRect = {0.0f, 0.0f, (float)currentBackground.width, (float)currentBackground.height};
    Rectangle destRect = {UI_CenterImg_X(SCREEN_W, CENTER_X), UI_CenterImg_Y(SCREEN_W / 3.0f * 2.0f, SCREEN_H / 2), SCREEN_W, SCREEN_W / 3.0f * 2.0f};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(currentBackground, sourceRect, destRect, origin, 0.0f, Fade(WHITE, 0.4));

    DrawRectangle(0, 0, SCREEN_W, START_SECTION_Y, Fade(themeColor2, 0.7));

    Vector2 startSection1 = {0, START_SECTION_Y};
    Vector2 endSection1 = {SCREEN_W, START_SECTION_Y};
    DrawLineEx(startSection1, endSection1, THICKNESS_LINE, themeColor3);
    Vector2 startSection2 = {0, END_SECTION_Y};
    Vector2 endSection2 = {SCREEN_W, END_SECTION_Y};
    DrawLineEx(startSection2, endSection2, THICKNESS_LINE, themeColor3);
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
    DrawTextEx(fontBold, DIAGNOSTICS_TITLE, diagnosticsTitle_Text, DIAGNOSTICS_TITLE_SIZE, BUMPER_TXT_SPACE, themeColor3);

    Vector2 diagnosticsResolution_Text = {RESOLUTION_TXT_X, RESOLUTION_TXT_Y};
    DrawTextEx(fontRegular, TextFormat("Resolution:  %d:%d", GetMonitorWidth(0), GetMonitorHeight(0)), diagnosticsResolution_Text, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, themeColor3);
    Vector2 diagnosticsFPS_Text = {FPS_TXT_X, FPS_TXT_Y};
    DrawTextEx(fontRegular, TextFormat("FPS:  %d", GetFPS()), diagnosticsFPS_Text, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, themeColor3);

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
        DrawTextEx(fontRegular, TextFormat("Resolution:  %d:%d", GetMonitorWidth(0), GetMonitorHeight(0)), diagnosticsResolution_Text, DIAG_SIZE, DIAG_SPACING, themeColor3);
        Vector2 diagnosticsFPS_Text = {DIAG_FPS_X, DIAG_FPS_Y};
        DrawTextEx(fontRegular, TextFormat("FPS:  %d", GetFPS()), diagnosticsFPS_Text, DIAG_SIZE, DIAG_SPACING, themeColor3);
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
    DrawRectangleRoundedLinesEx(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, 1.0f));
    DrawRectangleRounded(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, Fade(themeColor2, 1.0f));
    // Draw title
    Vector2 brightnessTxt_Size = MeasureTextEx(fontBold, BRIGHTNESS_TXT, BRIGHTNESS_TXT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 brightnessTxt = {BRIGHTNESS_TXT_X, BRIGHTNESS_TXT_Y};
    DrawTextEx(fontBold, BRIGHTNESS_TXT, brightnessTxt, BRIGHTNESS_TXT_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));

    DrawLineEx((Vector2){BRIGHTNESS_LINE_X_START, BRIGHTNESS_LINE_Y}, (Vector2){BRIGHTNESS_LINE_X_END, BRIGHTNESS_LINE_Y}, BRIGHTNESS_LINE_THICK, themeColor1);

    // Transition
    switch (currentBrightPressed)
    {
    case NEITHER:
        if ((IsKeyDown(KEY_LEFT) || LS_LEFT) && brightness < MIN_BRIGHTNESS)
        {
            currentBrightPressed = LEFT_PRESS;
            debugTimer = 0.0f;
        }
        else if ((IsKeyDown(KEY_RIGHT) || LS_RIGHT) && brightness > MAX_BRIGHTNESS)
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
        if (brightness < MAX_BRIGHTNESS)
        {
            brightness = MAX_BRIGHTNESS;
        }
        else if (brightness > MIN_BRIGHTNESS)
        {
            brightness = MIN_BRIGHTNESS;
        }
        break;

    case LEFT_PRESS:
        debugTimer += GetFrameTime();
        brightness += BRIGHTNESS_INCREMENT;
        brightCircleX -= BRIGHTNESS_CIRCLE_INCREMENT;
        if (brightness >= MIN_BRIGHTNESS)
        {
            printf("TIME TO GO LOW: %f\n", debugTimer);
            brightness = MIN_BRIGHTNESS;
            brightCircleX = BRIGHTNESS_LINE_X_START;
        }
        Update_UI_Txt();
        break;

    case RIGHT_PRESS:
        debugTimer += GetFrameTime();
        brightness -= BRIGHTNESS_INCREMENT;
        brightCircleX += BRIGHTNESS_CIRCLE_INCREMENT;
        if (brightness <= MAX_BRIGHTNESS)
        {
            printf("TIME TO GO HIGH: %f\n", debugTimer);
            brightness = MAX_BRIGHTNESS;
            brightCircleX = BRIGHTNESS_CIRCLE_X;
        }
        Update_UI_Txt();
        break;
    }

    DrawCircle(brightCircleX, BRIGHTNESS_LINE_Y, BRIGHTNESS_RADIUS, themeColor1);

    DrawCircle(BRIGHT_LS_LEFT_X + (SCREEN_W / 700.0f), BRIGHT_LS_Y, BRIGHT_LS_RADIUS, Fade(themeColor3, alphaSelectTxt));
    Vector2 point1 = {BRIGHT_LS_LEFT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y};
    Vector2 point2 = {BRIGHT_LS_LEFT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y + BRIGHT_LS_TRIANGLE_SIZE / 2};
    Vector2 point3 = {BRIGHT_LS_LEFT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y - BRIGHT_LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point1, point2, point3, Fade(themeColor2, alphaSelectTxt));

    DrawCircle(BRIGHT_LS_RIGHT_X - (SCREEN_W / 700.0f), BRIGHT_LS_Y, BRIGHT_LS_RADIUS, Fade(themeColor3, alphaSelectTxt));
    Vector2 point4 = {BRIGHT_LS_RIGHT_X + (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y};
    Vector2 point5 = {BRIGHT_LS_RIGHT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y + BRIGHT_LS_TRIANGLE_SIZE / 2};
    Vector2 point6 = {BRIGHT_LS_RIGHT_X - (BRIGHT_LS_TRIANGLE_SIZE / 2), BRIGHT_LS_Y - BRIGHT_LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point6, point5, point4, Fade(themeColor2, alphaSelectTxt));

    int brightPercent = (int)((MIN_BRIGHTNESS - brightness) / (MIN_BRIGHTNESS / 100.0f));
    snprintf(brightnessPercent_Txt, sizeof(brightnessPercent_Txt), "%d%%", brightPercent);
    Vector2 brightnessPercent_Size = MeasureTextEx(fontBold, brightnessPercent_Txt, BRIGHTNESS_PERCENT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 brightnessPercent = {BRIGHTNESS_PERCENT_X, BRIGHTNESS_PERCENT_Y};
    DrawTextEx(fontBold, brightnessPercent_Txt, brightnessPercent, BRIGHTNESS_PERCENT_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));
}

void MainMenu_DrawTheme()
{
    // Draw whole section
    Rectangle rect1 = {
        OPTIONS_RECT_X,
        OPTIONS_RECT_Y,
        OPTIONS_RECT_W,
        OPTIONS_RECT_H};
    DrawRectangleRoundedLinesEx(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, 1.0f));
    DrawRectangleRounded(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, Fade(themeColor2, 1.0f));
    // Draw title
    Vector2 themeTxt_Size = MeasureTextEx(fontBold, THEME_TXT, THEME_TXT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 themeTxt = {THEME_TXT_X, THEME_TXT_Y};
    DrawTextEx(fontBold, THEME_TXT, themeTxt, THEME_TXT_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));

    Rectangle rect2 = {
        THEME_RECT_X,
        THEME_RECT_1_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLUE);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_1_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect2, THICKNESS_LINE, themeColor3);

    Rectangle rect3 = {
        THEME_RECT_X,
        THEME_RECT_2_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, RED);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_2_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect3, THICKNESS_LINE, themeColor3);

    Rectangle rect4 = {
        THEME_RECT_X,
        THEME_RECT_3_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, GREEN);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_3_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect4, THICKNESS_LINE, themeColor3);

    Rectangle rect5 = {
        THEME_RECT_X,
        THEME_RECT_4_Y,
        THEME_RECT_W,
        THEME_RECT_H};
    DrawRectangleRec((Rectangle){THEME_RECT_X, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, YELLOW);
    DrawRectangleRec((Rectangle){THEME_X2, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, BLACK);
    DrawRectangleRec((Rectangle){THEME_X3, THEME_RECT_4_Y, THEME_RECT_W / 3, THEME_RECT_H}, WHITE);
    DrawRectangleLinesEx(rect5, THICKNESS_LINE, themeColor3);

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
        DrawRectangleLinesEx(rect2, 2 * THICKNESS_LINE, themeColor1);
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS) && mouseLeftWasPressed == false)
        {
            themeColor1 = BLUE;
            themeColor2 = BLACK;
            themeColor3 = WHITE;
            currentBackground = background_Blue;
            Update_UI_Txt();
        }
        break;

    case THEME_2:
        themeTimeElapsed += GetFrameTime();
        DrawRectangleLinesEx(rect3, 2 * THICKNESS_LINE, themeColor1);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            themeColor1 = RED;
            themeColor2 = BLACK;
            themeColor3 = WHITE;
            currentBackground = background_Red;
            Update_UI_Txt();
        }
        break;

    case THEME_3:
        themeTimeElapsed += GetFrameTime();
        DrawRectangleLinesEx(rect4, 2 * THICKNESS_LINE, themeColor1);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            themeColor1 = GREEN;
            themeColor2 = BLACK;
            themeColor3 = WHITE;
            currentBackground = background_Green;
            Update_UI_Txt();
        }
        break;

    case THEME_4:
        themeTimeElapsed += GetFrameTime();
        DrawRectangleLinesEx(rect5, 2 * THICKNESS_LINE, themeColor1);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || A_PRESS)
        {
            themeColor1 = YELLOW;
            themeColor2 = BLACK;
            themeColor3 = WHITE;
            currentBackground = background_Yellow;
        }
        break;
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
    DrawRectangleRoundedLinesEx(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, 1.0f));
    DrawRectangleRounded(rect1, OPTIONS_ROUND, OPTIONS_SEGMENTS, Fade(themeColor2, 1.0f));
    // Draw title
    Vector2 optionsTitle_Size = MeasureTextEx(fontBold, OPTIONS_TITLE, OPTIONS_TITLE_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsTitle = {OPTIONS_TITLE_X, OPTIONS_TITLE_Y};
    DrawTextEx(fontBold, OPTIONS_TITLE, optionsTitle, OPTIONS_TITLE_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));
    // Draw view list
    Vector2 optionsList_Size = MeasureTextEx(fontRegular, OPTIONS_VIEW_LIST, OPTIONS_VIEW_LIST_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsList = {OPTIONS_VIEW_LIST_X, OPTIONS_VIEW_LIST_Y};
    Rectangle rectList = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_VIEW_LIST_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectList, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, 1.0f));
    DrawTextEx(fontRegular, OPTIONS_VIEW_LIST, optionsList, OPTIONS_VIEW_LIST_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));
    // Draw brightness section
    Vector2 optionsBrightness_Size = MeasureTextEx(fontRegular, OPTIONS_BRIGHTNESS, OPTIONS_BRIGHTNESS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsBrightness = {OPTIONS_BRIGHTNESS_X, OPTIONS_BRIGHTNESS_Y};
    Rectangle rectBright = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_BRIGHTNESS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectBright, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, 1.0f));
    DrawTextEx(fontRegular, OPTIONS_BRIGHTNESS, optionsBrightness, OPTIONS_BRIGHTNESS_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));
    // Draw theme section
    Vector2 optionsTheme_Size = MeasureTextEx(fontRegular, OPTIONS_THEME, OPTIONS_THEME_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsTheme = {OPTIONS_THEME_X, OPTIONS_THEME_Y};
    Rectangle rectTheme = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_THEME_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectTheme, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, 1.0f));
    DrawTextEx(fontRegular, OPTIONS_THEME, optionsTheme, OPTIONS_THEME_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));
    // Draw display diagnostics section
    Vector2 optionsDisplayDiagnostics_Size = MeasureTextEx(fontRegular, OPTIONS_DISPLAY_DIAGNOSTICS, OPTIONS_DISPLAY_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsDisplayDiagnostics = {OPTIONS_DISPLAY_DIAGNOSTICS_X, OPTIONS_DISPLAY_DIAGNOSTICS_Y};
    Rectangle rectDDiag = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_DISPLAY_DIAGNOSTICS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, 1.0f));
    char *diagTxt;
    if (displayDiag)
    {
        diagTxt = OPTIONS_HIDE_DIAGNOSTICS;
    }
    else
    {
        diagTxt = OPTIONS_DISPLAY_DIAGNOSTICS;
    }
    DrawTextEx(fontRegular, diagTxt, optionsDisplayDiagnostics, OPTIONS_DISPLAY_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));
    // Draw view diagnostics section
    Vector2 optionsDiagnostics_Size = MeasureTextEx(fontRegular, OPTIONS_DIAGNOSTICS, OPTIONS_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsDiagnostics = {OPTIONS_DIAGNOSTICS_X, OPTIONS_DIAGNOSTICS_Y};
    Rectangle rectDiag = {
        OPTIONS_SELECT_RECT_X,
        OPTIONS_DIAGNOSTICS_RECT_Y,
        OPTIONS_SELECT_RECT_W,
        OPTIONS_SELECT_RECT_H};
    DrawRectangleRoundedLinesEx(rectDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(themeColor3, 1.0f));
    char *menuTxt;
    if (currentConsoleState == STATE_VIEW_DIAG)
    {
        menuTxt = OPTIONS_MAIN;
    }
    else
    {
        menuTxt = OPTIONS_DIAGNOSTICS;
    }
    DrawTextEx(fontRegular, menuTxt, optionsDiagnostics, OPTIONS_DIAGNOSTICS_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));
    // Draw select text
    Vector2 optionsSelect_Size = MeasureTextEx(fontRegular, OPTIONS_SELECT, OPTIONS_SELECT_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsSelect = {OPTIONS_SELECT_X, OPTIONS_SELECT_Y};
    DrawTextEx(fontRegular, OPTIONS_SELECT, optionsSelect, OPTIONS_SELECT_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));
    // Draw back text
    Vector2 optionsBack_Size = MeasureTextEx(fontRegular, OPTIONS_BACK, OPTIONS_BACK_SIZE, OPTIONS_TITLE_SPACE);
    Vector2 optionsBack = {OPTIONS_BACK_X, OPTIONS_BACK_Y};
    DrawTextEx(fontRegular, OPTIONS_BACK, optionsBack, OPTIONS_BACK_SIZE, OPTIONS_TITLE_SPACE, Fade(themeColor3, 1.0f));

    if (currentOptionsState == LIST)
    {
        DrawRectangleRoundedLinesEx(rectList, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(themeColor1, 1.0f));
    }
    else if (currentOptionsState == BRIGHTNESS)
    {
        DrawRectangleRoundedLinesEx(rectBright, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(themeColor1, 1.0f));
    }
    else if (currentOptionsState == THEME)
    {
        DrawRectangleRoundedLinesEx(rectTheme, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(themeColor1, 1.0f));
    }
    else if (currentOptionsState == DISPLAY_DIAGNOSTICS)
    {
        DrawRectangleRoundedLinesEx(rectDDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(themeColor1, 1.0f));
    }
    else
    {
        DrawRectangleRoundedLinesEx(rectDiag, BOTTOM_ROUND, BOTTOM_SEGMENTS, 2 * THICKNESS_LINE, Fade(themeColor1, 1.0f));
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
        if (scrollGames != SCROLL_LEFT && scrollCategories == SCROLL_NO && (IsKeyDown(KEY_RIGHT) || LS_RIGHT))
        {
            // Scroll games to the right
            scrollGames = SCROLL_RIGHT;
            currentMainMenuState = SCROLL_GAMES;
        }
        // If not scrolling games to right, not scrolling categories, and the right input is pressed
        else if (scrollGames != SCROLL_RIGHT && scrollCategories == SCROLL_NO && (IsKeyDown(KEY_LEFT) || LS_LEFT))
        {
            // Scroll games to the left
            scrollGames = SCROLL_LEFT;
            currentMainMenuState = SCROLL_GAMES;
        }
        // If not scrolling categories to left and right input is pressed
        if (scrollCategories != SCROLL_LEFT && (IsKeyDown(KEY_D) || RB_HOLD))
        {
            // If not already scrolling cateogries
            if (scrollCategories == SCROLL_NO)
            {
                // Scroll cateogries to right and don't scroll games
                scrollCategories = SCROLL_RIGHT;
                scrollGames = SCROLL_NO;
                currentMainMenuState = SCROLL_CATEGORIES;
                // Get new games ready and reset the coordinates
                Game_New_Indexes();
                Games_NewRefresh();
                UI_ResetDisplayCoords_Scroll();
            }
        }
        // If not scrolling categories to right and right input is pressed
        else if (scrollCategories != SCROLL_RIGHT && (IsKeyDown(KEY_A) || LB_HOLD))
        {
            // If not already scrolling cateogries
            if (scrollCategories == SCROLL_NO)
            {
                // Scroll cateogries to right and don't scroll games
                scrollCategories = SCROLL_LEFT;
                scrollGames = SCROLL_NO;
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
        if (scrollGames == SCROLL_NO)
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
            scrollGames = SCROLL_NO;
            // Update displayed games
            Games_ScrollLeft();
            // Reset game values
            UI_ResetDisplayCoords_Games();
            scrollCategories = SCROLL_LEFT;
            currentMainMenuState = SCROLL_CATEGORIES;
            Game_New_Indexes();
            Games_NewRefresh();
            UI_ResetDisplayCoords_Scroll();
        }
        else if (IsKeyPressed(KEY_D) || RB_PRESS)
        {
            scrollGames = SCROLL_NO;
            // Update displayed games
            Games_ScrollRight();
            // Reset game values
            UI_ResetDisplayCoords_Games();
            scrollCategories = SCROLL_RIGHT;
            currentMainMenuState = SCROLL_CATEGORIES;
            Game_New_Indexes();
            Games_NewRefresh();
            UI_ResetDisplayCoords_Scroll();
        }
        break;

    case SCROLL_CATEGORIES:
        if (scrollCategories == SCROLL_NO)
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
            Fade(themeColor1, alphaGames));
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
        if (scrollGames != SCROLL_NO)
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
                Fade(themeColor1, alphaGames));
        }
        else if (scrollCategories != SCROLL_NO)
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