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

Font fontRegular;
Font fontBold;


Texture2D img;
Texture2D btnA;
Texture2D btnX;
char *title;
int title_X, title_Y, title_Size;
int img_X, img_Y, img_W, img_H;

enum Scroll scrollGames = SCROLL_NO;
enum Scroll scrollCategories = SCROLL_NO;

float alphaGames;
float alphaCategories_Out;
float alphaCategories_In;


//Change the alpha value for fading the display
void UI_ChangeAlpha_Games(float offRate, float onRate) {
    if (scrollGames == SCROLL_NO && scrollCategories == SCROLL_NO) {
        if (alphaGames <= 1.0f) {
            alphaGames += onRate;
        } 
        else {
            alphaGames = 1.0f;
        }
    }
    else {
        if (alphaGames <= 0.0f) {
            alphaGames = 0.0f;
        }
        else {
            alphaGames -= offRate;
        }
    }
}

void UI_ChangeAlpha_Categories(float offRate, float onRate) {
    if (scrollCategories != SCROLL_NO) {
        if (alphaCategories_In > 1.0f) {
            alphaCategories_In = 1.0f;
        }
        else {
            alphaCategories_In += onRate;
        }
        if (alphaCategories_Out < 0.0f) {
            alphaCategories_Out = 0.0f;
        }
        else {
            alphaCategories_Out -= offRate;
        }
    }
}

//Reset the coordinates for the displayed games
void UI_ResetDisplayCoords_Games() {

    gamesDisplayed[0]->x = LEFT3_GAME_X;
    gamesDisplayed[1]->x = LEFT2_GAME_X;
    gamesDisplayed[2]->x = LEFT1_GAME_X;
    gamesDisplayed[3]->x = CENTER_GAME_X;
    gamesDisplayed[4]->x = RIGHT1_GAME_X;
    gamesDisplayed[5]->x = RIGHT2_GAME_X;
    gamesDisplayed[6]->x = RIGHT3_GAME_X;

    gamesDisplayed[0]->y = SIDE3_GAME_Y;
    gamesDisplayed[1]->y = SIDE2_GAME_Y;
    gamesDisplayed[2]->y = SIDE1_GAME_Y;
    gamesDisplayed[3]->y = CENTER_GAME_Y;
    gamesDisplayed[4]->y = SIDE1_GAME_Y;
    gamesDisplayed[5]->y = SIDE2_GAME_Y;
    gamesDisplayed[6]->y = SIDE3_GAME_Y;

    gamesDisplayed[0]->w = SIDE3_GAME_W;
    gamesDisplayed[1]->w = SIDE2_GAME_W;
    gamesDisplayed[2]->w = SIDE1_GAME_W;
    gamesDisplayed[3]->w = CENTER_GAME_W;
    gamesDisplayed[4]->w = SIDE1_GAME_W;
    gamesDisplayed[5]->w = SIDE2_GAME_W;
    gamesDisplayed[6]->w = SIDE3_GAME_W;

    gamesDisplayed[0]->h = SIDE3_GAME_H;
    gamesDisplayed[1]->h = SIDE2_GAME_H;
    gamesDisplayed[2]->h = SIDE1_GAME_H;
    gamesDisplayed[3]->h = CENTER_GAME_H;
    gamesDisplayed[4]->h = SIDE1_GAME_H;
    gamesDisplayed[5]->h = SIDE2_GAME_H;
    gamesDisplayed[6]->h = SIDE3_GAME_H;
 
}

void UI_ResetDisplayCoords_Scroll() {
    if (scrollCategories == SCROLL_RIGHT) {
        newGamesDisplayed[0]->x = LEFT2_SCROLLL_X;
        newGamesDisplayed[1]->x = LEFT1_SCROLLL_X;
        newGamesDisplayed[2]->x = CENTER_SCROLLL_X;
        newGamesDisplayed[3]->x = RIGHT1_SCROLLL_X;
        newGamesDisplayed[4]->x = RIGHT2_SCROLLL_X;
    }
    else {
        newGamesDisplayed[0]->x = LEFT2_SCROLLR_X;
        newGamesDisplayed[1]->x = LEFT1_SCROLLR_X;
        newGamesDisplayed[2]->x = CENTER_SCROLLR_X;
        newGamesDisplayed[3]->x = RIGHT1_SCROLLR_X;
        newGamesDisplayed[4]->x = RIGHT2_SCROLLR_X;
    }

    newGamesDisplayed[0]->y = SIDE2_GAME_Y;
    newGamesDisplayed[1]->y = SIDE1_GAME_Y;
    newGamesDisplayed[2]->y = CENTER_GAME_Y;
    newGamesDisplayed[3]->y = SIDE1_GAME_Y;
    newGamesDisplayed[4]->y = SIDE2_GAME_Y;

    newGamesDisplayed[0]->w = SIDE2_GAME_W;
    newGamesDisplayed[1]->w = SIDE1_GAME_W;
    newGamesDisplayed[2]->w = CENTER_GAME_W;
    newGamesDisplayed[3]->w = SIDE1_GAME_W;
    newGamesDisplayed[4]->w = SIDE2_GAME_W;

    newGamesDisplayed[0]->h = SIDE2_GAME_H;
    newGamesDisplayed[1]->h = SIDE1_GAME_H;
    newGamesDisplayed[2]->h = CENTER_GAME_H;
    newGamesDisplayed[3]->h = SIDE1_GAME_H;
    newGamesDisplayed[4]->h = SIDE2_GAME_H;
}

//Function for drawing an image
void UI_DrawImage(float alpha) {

    if (img.id > 0) {
            Rectangle sourceRect = {0.0f, 0.0f, (float)img.width, (float)img.height};
            Rectangle destRect = {(float)img_X, img_Y, img_W, img_H};
            Vector2 origin = {0.0f, 0.0f};
            DrawTexturePro(img, sourceRect, destRect, origin, 0.0f, Fade(WHITE, alpha));
        } 
        else {
            // Draw a gray placeholder box if the image file is missing!
            DrawRectangle(img_X, img_Y, img_W, img_H, Fade(LIGHTGRAY, alpha));
            DrawRectangleLines(img_X, img_Y, img_W, img_H, Fade(GRAY, alpha));
        }
}

//Function for drawing a game cover
void UI_DrawGame(int i, float alpha, bool scroll) {
    if (scroll) {
        img_Y = newGamesDisplayed[i]->y;
        img_W = newGamesDisplayed[i]->w;
        img_H = newGamesDisplayed[i]->h;
        img_X = newGamesDisplayed[i]->x;
        img = newGamesDisplayed[i]->cover;

        UI_DrawImage(alpha);
        DrawRectangleLinesEx(
        (Rectangle){
            img_X - THICKNESS_OTHER,
            img_Y - THICKNESS_OTHER,
            img_W + (2 * THICKNESS_OTHER),
            img_H + (2 * THICKNESS_OTHER)
        },
        THICKNESS_OTHER, 
        Fade(GRAY, alphaCategories_In)
    );
    }
    else {
        img_Y = gamesDisplayed[i]->y;
        img_W = gamesDisplayed[i]->w;
        img_H = gamesDisplayed[i]->h;
        img_X = gamesDisplayed[i]->x;
        img = gamesDisplayed[i]->cover;

        UI_DrawImage(alpha);
        DrawRectangleLinesEx(
        (Rectangle){
            img_X - THICKNESS_OTHER,
            img_Y - THICKNESS_OTHER,
            img_W + (2 * THICKNESS_OTHER),
            img_H + (2 * THICKNESS_OTHER)
        },
        THICKNESS_OTHER, 
        Fade(GRAY, alphaCategories_Out)
    );
    }
 
}

//Function for drawing the games when scrolling to the left
void UI_DrawGames_Left() {
    UI_DrawGame(0, 1.0f, false);
    UI_DrawGame(1, 1.0f, false);
    UI_DrawGame(5, 1.0f, false);
    UI_DrawGame(4, 1.0f, false);
    UI_DrawGame(3, 1.0f, false);
    UI_DrawGame(2, 1.0f, false);
}

//Function for drawing the games when scrolling to the right
void UI_DrawGames_Right() {
    UI_DrawGame(6, 1.0f, false);
    UI_DrawGame(1, 1.0f, false);
    UI_DrawGame(5, 1.0f, false);
    UI_DrawGame(2, 1.0f, false);
    UI_DrawGame(3, 1.0f, false);
    UI_DrawGame(4, 1.0f, false);
}

//Function for drawing the animation of scrolling games
void UI_DrawScroll_Games() {

    if (scrollGames == SCROLL_RIGHT) {
        UI_DrawGames_Right();
        if (fabsf(gamesDisplayed[4]->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD) {
            scrollGames = SCROLL_NO;
            Games_ScrollRight();
            UI_ResetDisplayCoords_Games();
            return;
        }

        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT3_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED); 
        gamesDisplayed[6]->x = Lerp(gamesDisplayed[6]->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED); 

        gamesDisplayed[1]->y = Lerp(gamesDisplayed[1]->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->y = Lerp(gamesDisplayed[2]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->y = Lerp(gamesDisplayed[3]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->y = Lerp(gamesDisplayed[4]->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->y = Lerp(gamesDisplayed[5]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[6]->y = Lerp(gamesDisplayed[6]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);   

        gamesDisplayed[1]->w = Lerp(gamesDisplayed[1]->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->w = Lerp(gamesDisplayed[2]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->w = Lerp(gamesDisplayed[3]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->w = Lerp(gamesDisplayed[4]->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->w = Lerp(gamesDisplayed[5]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED); 
        gamesDisplayed[6]->w = Lerp(gamesDisplayed[6]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);

        gamesDisplayed[1]->h = Lerp(gamesDisplayed[1]->h, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->h = Lerp(gamesDisplayed[2]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->h = Lerp(gamesDisplayed[3]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->h = Lerp(gamesDisplayed[4]->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->h = Lerp(gamesDisplayed[5]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);  
        gamesDisplayed[6]->h = Lerp(gamesDisplayed[6]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
    }
    
    else if (scrollGames == SCROLL_LEFT) {
        UI_DrawGames_Left();
        if (fabsf(gamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD) {
            scrollGames = SCROLL_NO;
            Games_ScrollLeft();
            UI_ResetDisplayCoords_Games();
            return;
        }
        
        gamesDisplayed[0]->x = Lerp(gamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT3_GAME_X, SCROLL_GAMES_SPEED); 

        gamesDisplayed[0]->y = Lerp(gamesDisplayed[0]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->y = Lerp(gamesDisplayed[1]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->y = Lerp(gamesDisplayed[2]->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->y = Lerp(gamesDisplayed[3]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->y = Lerp(gamesDisplayed[4]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED); 
        gamesDisplayed[5]->y = Lerp(gamesDisplayed[5]->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED); 

        gamesDisplayed[0]->w = Lerp(gamesDisplayed[0]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->w = Lerp(gamesDisplayed[1]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->w = Lerp(gamesDisplayed[2]->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->w = Lerp(gamesDisplayed[3]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->w = Lerp(gamesDisplayed[4]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->w = Lerp(gamesDisplayed[5]->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);

        gamesDisplayed[0]->h = Lerp(gamesDisplayed[0]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->h = Lerp(gamesDisplayed[1]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->h = Lerp(gamesDisplayed[2]->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->h = Lerp(gamesDisplayed[3]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->h = Lerp(gamesDisplayed[4]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->h = Lerp(gamesDisplayed[5]->h, SIDE3_GAME_H, SCROLL_GAMES_SPEED);
    }
}

//Function for drawing games depending on whether it is scrolling
void UI_DrawGames() {
    if (scrollGames == SCROLL_NO) {
        if (scrollCategories != SCROLL_NO) {
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
    }
    else {
        UI_DrawScroll_Games();
    }
        DrawRectangleLinesEx(
            (Rectangle){
                img_X - THICKNESS_SELECT_GAME,
                img_Y - THICKNESS_SELECT_GAME,
                img_W + (2 * THICKNESS_SELECT_GAME),
                img_H + (2 * THICKNESS_SELECT_GAME)
            },
            THICKNESS_SELECT_GAME, 
            Fade(BLUE, alphaGames)
        );
    
}

//Function for drawing the cateogires when they are static
void UI_DrawCategories_Normal() {
    Vector2 size = MeasureTextEx(fontBold, categoriesDisplayed[2].name, CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 centerCateg = {CENTER_CATEG_X, CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[2].name, centerCateg, CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaCategories_Out));

    size = MeasureTextEx(fontBold, categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 left1Categ = {LEFT1_CATEG_X, CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[1].name, left1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaCategories_Out));


    size = MeasureTextEx(fontBold, categoriesDisplayed[3].name, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 right1Categ = {RIGHT1_CATEG_X, CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[3].name, right1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaCategories_Out));
}

//Function for drawing the animation of scrolling categories
void UI_DrawScroll_Categories() {
    if (scrollCategories == SCROLL_RIGHT) {
        if (fabsf(newGamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD) {
            Categories_ScrollRight();
            UI_DrawCategories_Normal();
            UI_DrawGames();
            scrollCategories = SCROLL_NO;
            alphaCategories_Out = 1.0f;
            alphaCategories_In = 0.0f;
            UI_ResetDisplayCoords_Games();
            return;
        }

        newGamesDisplayed[0]->x = Lerp(newGamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[1]->x = Lerp(newGamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[2]->x = Lerp(newGamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[3]->x = Lerp(newGamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[4]->x = Lerp(newGamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);

        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, LEFT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, CENTER_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, RIGHT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
    }
    else {
        if (fabsf(newGamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD) {
            Categories_ScrollLeft();
            UI_DrawCategories_Normal();
            UI_DrawGames();
            scrollCategories = SCROLL_NO;
            UI_ResetDisplayCoords_Games();
            alphaCategories_Out = 1.0f;
            alphaCategories_Out = 1.0f;
            alphaCategories_In = 0.0f;
            UI_ResetDisplayCoords_Games();
            return;
        }

        newGamesDisplayed[0]->x = Lerp(newGamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[1]->x = Lerp(newGamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[2]->x = Lerp(newGamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[3]->x = Lerp(newGamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[4]->x = Lerp(newGamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);

        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT2_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, LEFT1_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, CENTER_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, RIGHT1_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT2_SCROLLL_X, SCROLL_CATEG_OUT_SPEED);
    }
}

//Function for centering an image on a certain X position
float UI_CenterImg_X(float width, float position) {
    return (position - width / 2);
}

//Function for centering an image on a certain Y position
float UI_CenterImg_Y(float height, float position) {
    return (position - height / 2.0f);
}

//Function to center text on a certain X position
float UI_CenterText_X(char *text, int fontSize, int position) {
    float width = MeasureText(text, fontSize);
    return (position - (width / 2));
}

//Function to draw the bumpers
void UI_DrawBumpers() {
    Rectangle bumperL = {
        BUMPER_L_X,
        BUMPER_Y,
        BUMPER_W,
        BUMPER_H
    };

    Rectangle bumperR = {
        BUMPER_R_X,
        BUMPER_Y,
        BUMPER_W,
        BUMPER_H
    };

    DrawRectangleRounded(bumperL, BUMPER_ROUND, BUMPER_SEGMENTS, LIGHTGRAY);
    DrawRectangleRounded(bumperR, BUMPER_ROUND, BUMPER_SEGMENTS, LIGHTGRAY);

    Vector2 bumperL_Size = MeasureTextEx(fontBold, BUMPER_L_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperR_Size = MeasureTextEx(fontBold, BUMPER_R_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperR_Text = {BUMPER_R_TXT_X, BUMPER_TXT_Y};
    Vector2 bumperL_Text = {BUMPER_L_TXT_X, BUMPER_TXT_Y};
    DrawTextEx(fontBold, BUMPER_R_TXT, bumperR_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, BLACK);
    DrawTextEx(fontBold, BUMPER_L_TXT, bumperL_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, BLACK);
}

//Function for drawing the select controls at the bottom
void UI_DrawBottom() {
    
    Vector2 gameText_Size = MeasureTextEx(fontRegular, gamesDisplayed[3]->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 bottomText_Size = MeasureTextEx(fontRegular, BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 gameText = {BOTTOM_TXT_X, GAME_TXT_Y};
    Vector2 bottomText = {BOTTOM_TXT_X, BOTTOM_TXT_Y};

    DrawRectangle(BOTTOM_RECT_X, BOTTOM_RECT_Y, BOTTOM_RECT_W, BOTTOM_RECT_H, Fade(WHITE, alphaGames));

    DrawTextEx(fontRegular, gamesDisplayed[3]->title, gameText, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(BLACK, alphaGames));
    DrawTextEx(fontRegular, BOTTOM_TXT, bottomText, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(BLACK, alphaGames));
}

//Function for drawing the boot screen
void UI_DrawBootScreen() {
    ClearBackground(BACKGROUND_CLR);
    /*DrawCircle(400, 225, 200, RED);
    DrawText("Welcome", 250, 185, 75, BLUE);*/

    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(LOGO_SIZE, CENTER_X), UI_CenterImg_Y(LOGO_SIZE, SCREEN_H / 2), LOGO_SIZE, LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Function for drawing the heading at the top of the screen
void UI_DrawHeading() {
    //DrawText(COMPANY_NAME, UI_CenterText_X(COMPANY_NAME, COMPANY_TXT_SIZE, CENTER_X), COMPANY_Y, COMPANY_TXT_SIZE, BLUE);
    //DrawText(PICK_GAME_TXT, UI_CenterText_X(PICK_GAME_TXT, PICK_GAME_TXT_SIZE, CENTER_X), PICK_GAME_Y, PICK_GAME_TXT_SIZE, BLUE);
    //DrawRectangle(0, 0, SCREEN_W, TOP_Y, WHITE);
    //DrawLine(0, TOP_Y, SCREEN_W, TOP_Y, BLUE);
}

//Function for drawing categories depending on whether it is scrolling
void UI_DrawCategories() {
    if (scrollCategories == SCROLL_NO) {
        UI_DrawCategories_Normal();
    }
    else {
        UI_DrawScroll_Categories();
    }
}

//Function for drawing the main menu
void UI_DrawMainMenu() {
    ClearBackground(BACKGROUND_CLR);

    UI_DrawHeading();
    UI_ChangeAlpha_Games(0.25f, 0.25f);
    UI_ChangeAlpha_Categories(0.1f, 0.05f);
    UI_DrawCategories();
    UI_DrawGames();
    UI_DrawBumpers();

    UI_DrawBottom();
}

//Function for drawing the diagnostics screen
void UI_DrawDiagnostics() {
    return;
}
