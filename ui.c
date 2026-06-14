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

//Types of font
Font fontRegular;
Font fontBold;
//Variable to hold the current image to draw
Texture2D img;
//Variables to hold information about the current image to draw
int img_X, img_Y, img_W, img_H;
//Different alphas
float alphaGames;
float alphaCategories_Out;
float alphaCategories_In;
//Variables to show whether user is scrolling
Scroll scrollGames;
Scroll scrollCategories;



//Change the alpha value for fading when scrolling games
void UI_ChangeAlpha_Games(float offRate, float onRate) {
    //If not scrolling at all, increment alpha back to normal
    if (scrollGames == SCROLL_NO && scrollCategories == SCROLL_NO) {
        //If less than 1, increment 
        if (alphaGames < 1.0f) {
            alphaGames += onRate;
        } 
        //If greater than 1, bring back to 1
        else if (alphaGames > 1.0f) {
            alphaGames = 1.0f;
        }
    }
    //If scrolling, bring alpha down
    else {
        //If less than 0, bring to 0
        if (alphaGames < 0.0f) {
            alphaGames = 0.0f;
        }
        //If greater than 0, decrement
        else if (alphaGames > 0.0f) {
            alphaGames -= offRate;
        }
    }
}

//Change the alpha value for fading the games during category scroll
void UI_ChangeAlpha_Categories(float offRate, float onRate) {
    //If scrolling categories
    if (scrollCategories != SCROLL_NO) {
        //If greater than 1, bring to 1
        if (alphaCategories_In > 1.0f) {
            alphaCategories_In = 1.0f;
        }
        //If less than 1, increment up
        else if (alphaCategories_In < 1.0f) {
            alphaCategories_In += onRate;
        }
        //If less than 0, bring to 0
        if (alphaCategories_Out < 0.0f) {
            alphaCategories_Out = 0.0f;
        }
        //If greater than 0, decrement
        else if (alphaCategories_Out > 0.0f) {
            alphaCategories_Out -= offRate;
        }
    }
}

//Reset the coordinates for the displayed games after games scroll
void UI_ResetDisplayCoords_Games() {
    //Reset x coordinates
    gamesDisplayed[0]->x = LEFT3_GAME_X;
    gamesDisplayed[1]->x = LEFT2_GAME_X;
    gamesDisplayed[2]->x = LEFT1_GAME_X;
    gamesDisplayed[3]->x = CENTER_GAME_X;
    gamesDisplayed[4]->x = RIGHT1_GAME_X;
    gamesDisplayed[5]->x = RIGHT2_GAME_X;
    gamesDisplayed[6]->x = RIGHT3_GAME_X;
    //Reset y coordinates
    gamesDisplayed[0]->y = SIDE3_GAME_Y;
    gamesDisplayed[1]->y = SIDE2_GAME_Y;
    gamesDisplayed[2]->y = SIDE1_GAME_Y;
    gamesDisplayed[3]->y = CENTER_GAME_Y;
    gamesDisplayed[4]->y = SIDE1_GAME_Y;
    gamesDisplayed[5]->y = SIDE2_GAME_Y;
    gamesDisplayed[6]->y = SIDE3_GAME_Y;
    //Reset width values
    gamesDisplayed[0]->w = SIDE3_GAME_W;
    gamesDisplayed[1]->w = SIDE2_GAME_W;
    gamesDisplayed[2]->w = SIDE1_GAME_W;
    gamesDisplayed[3]->w = CENTER_GAME_W;
    gamesDisplayed[4]->w = SIDE1_GAME_W;
    gamesDisplayed[5]->w = SIDE2_GAME_W;
    gamesDisplayed[6]->w = SIDE3_GAME_W;
    //Reset height values
    gamesDisplayed[0]->h = SIDE3_GAME_H;
    gamesDisplayed[1]->h = SIDE2_GAME_H;
    gamesDisplayed[2]->h = SIDE1_GAME_H;
    gamesDisplayed[3]->h = CENTER_GAME_H;
    gamesDisplayed[4]->h = SIDE1_GAME_H;
    gamesDisplayed[5]->h = SIDE2_GAME_H;
    gamesDisplayed[6]->h = SIDE3_GAME_H;
}

//Reset the coordinates for the displayed games after category scroll
void UI_ResetDisplayCoords_Scroll() {
    //If scrolling to the right
    if (scrollCategories == SCROLL_RIGHT) {
        //Reset x coordinates
        newGamesDisplayed[0]->x = LEFT2_SCROLLL_X;
        newGamesDisplayed[1]->x = LEFT1_SCROLLL_X;
        newGamesDisplayed[2]->x = CENTER_SCROLLL_X;
        newGamesDisplayed[3]->x = RIGHT1_SCROLLL_X;
        newGamesDisplayed[4]->x = RIGHT2_SCROLLL_X;
    }
    //If scrolling to the left
    else {
        //Reset x coordinates
        newGamesDisplayed[0]->x = LEFT2_SCROLLR_X;
        newGamesDisplayed[1]->x = LEFT1_SCROLLR_X;
        newGamesDisplayed[2]->x = CENTER_SCROLLR_X;
        newGamesDisplayed[3]->x = RIGHT1_SCROLLR_X;
        newGamesDisplayed[4]->x = RIGHT2_SCROLLR_X;
    }
    //Reset y coordinates
    newGamesDisplayed[0]->y = SIDE2_GAME_Y;
    newGamesDisplayed[1]->y = SIDE1_GAME_Y;
    newGamesDisplayed[2]->y = CENTER_GAME_Y;
    newGamesDisplayed[3]->y = SIDE1_GAME_Y;
    newGamesDisplayed[4]->y = SIDE2_GAME_Y;
    //Reset width values
    newGamesDisplayed[0]->w = SIDE2_GAME_W;
    newGamesDisplayed[1]->w = SIDE1_GAME_W;
    newGamesDisplayed[2]->w = CENTER_GAME_W;
    newGamesDisplayed[3]->w = SIDE1_GAME_W;
    newGamesDisplayed[4]->w = SIDE2_GAME_W;
    //Reset height values
    newGamesDisplayed[0]->h = SIDE2_GAME_H;
    newGamesDisplayed[1]->h = SIDE1_GAME_H;
    newGamesDisplayed[2]->h = CENTER_GAME_H;
    newGamesDisplayed[3]->h = SIDE1_GAME_H;
    newGamesDisplayed[4]->h = SIDE2_GAME_H;
}

//Function for drawing an image
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

//Function for drawing a game cover
void UI_DrawGame(int i, float alpha, bool scroll) {
    //If scrolling
    if (scroll) {
        //Update values of image to draw
        img_Y = newGamesDisplayed[i]->y;
        img_W = newGamesDisplayed[i]->w;
        img_H = newGamesDisplayed[i]->h;
        img_X = newGamesDisplayed[i]->x;
        img = newGamesDisplayed[i]->cover;
        //Draw image
        UI_DrawImage(alpha);
        //Draw outline
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
    //If not scrolling
    else {
        //Update values of image to draw
        img_Y = gamesDisplayed[i]->y;
        img_W = gamesDisplayed[i]->w;
        img_H = gamesDisplayed[i]->h;
        img_X = gamesDisplayed[i]->x;
        img = gamesDisplayed[i]->cover;
        //Draw image
        UI_DrawImage(alpha);
        //Draw outline
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
    //If scrolling to the right
    if (scrollGames == SCROLL_RIGHT) {
        //Draw the games
        UI_DrawGames_Right();
        //If images close enough to final spot
        if (fabsf(gamesDisplayed[4]->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD) {
            scrollGames = SCROLL_NO;
            //Update displayed games
            Games_ScrollRight();
            //Reset game values
            UI_ResetDisplayCoords_Games();
            return;
        }
        //Update x coordinates
        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT3_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED); 
        gamesDisplayed[6]->x = Lerp(gamesDisplayed[6]->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED); 
        //Update y coordinates
        gamesDisplayed[1]->y = Lerp(gamesDisplayed[1]->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->y = Lerp(gamesDisplayed[2]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->y = Lerp(gamesDisplayed[3]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->y = Lerp(gamesDisplayed[4]->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->y = Lerp(gamesDisplayed[5]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[6]->y = Lerp(gamesDisplayed[6]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);   
        //Update width values
        gamesDisplayed[1]->w = Lerp(gamesDisplayed[1]->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->w = Lerp(gamesDisplayed[2]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->w = Lerp(gamesDisplayed[3]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->w = Lerp(gamesDisplayed[4]->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->w = Lerp(gamesDisplayed[5]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED); 
        gamesDisplayed[6]->w = Lerp(gamesDisplayed[6]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        //Update height values
        gamesDisplayed[1]->h = Lerp(gamesDisplayed[1]->h, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->h = Lerp(gamesDisplayed[2]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->h = Lerp(gamesDisplayed[3]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->h = Lerp(gamesDisplayed[4]->h, CENTER_GAME_H, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->h = Lerp(gamesDisplayed[5]->h, SIDE1_GAME_H, SCROLL_GAMES_SPEED);  
        gamesDisplayed[6]->h = Lerp(gamesDisplayed[6]->h, SIDE2_GAME_H, SCROLL_GAMES_SPEED);
    }
    //If scrolling to the left
    else if (scrollGames == SCROLL_LEFT) {
        //Draw the games
        UI_DrawGames_Left();
        //If images close enough to final spot
        if (fabsf(gamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_GAMES_THRESHOLD) {
            scrollGames = SCROLL_NO;
            //Update displayed games
            Games_ScrollLeft();
            //Reset game values
            UI_ResetDisplayCoords_Games();
            return;
        }
        //Update x coordinates
        gamesDisplayed[0]->x = Lerp(gamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT3_GAME_X, SCROLL_GAMES_SPEED); 
        //Update y coordinates
        gamesDisplayed[0]->y = Lerp(gamesDisplayed[0]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->y = Lerp(gamesDisplayed[1]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->y = Lerp(gamesDisplayed[2]->y, CENTER_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->y = Lerp(gamesDisplayed[3]->y, SIDE1_GAME_Y, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->y = Lerp(gamesDisplayed[4]->y, SIDE2_GAME_Y, SCROLL_GAMES_SPEED); 
        gamesDisplayed[5]->y = Lerp(gamesDisplayed[5]->y, SIDE3_GAME_Y, SCROLL_GAMES_SPEED); 
        //Update width values
        gamesDisplayed[0]->w = Lerp(gamesDisplayed[0]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[1]->w = Lerp(gamesDisplayed[1]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[2]->w = Lerp(gamesDisplayed[2]->w, CENTER_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[3]->w = Lerp(gamesDisplayed[3]->w, SIDE1_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[4]->w = Lerp(gamesDisplayed[4]->w, SIDE2_GAME_W, SCROLL_GAMES_SPEED);
        gamesDisplayed[5]->w = Lerp(gamesDisplayed[5]->w, SIDE3_GAME_W, SCROLL_GAMES_SPEED);
        //Update height values
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
    //If not scrolling
    if (scrollGames == SCROLL_NO) {
        //If scrolling categories
        if (scrollCategories != SCROLL_NO) {
            UI_DrawGame(0, alphaCategories_In, true);
            UI_DrawGame(4, alphaCategories_In, true);
            UI_DrawGame(1, alphaCategories_In, true);
            UI_DrawGame(3, alphaCategories_In, true); 
            UI_DrawGame(2, alphaCategories_In, true);
        }
        //If not scrolling categories
        UI_DrawGame(1, alphaCategories_Out, false);
        UI_DrawGame(5, alphaCategories_Out, false);
        UI_DrawGame(2, alphaCategories_Out, false);
        UI_DrawGame(4, alphaCategories_Out, false); 
        UI_DrawGame(3, alphaCategories_Out, false);
    }
    //If scrolling
    else {
        UI_DrawScroll_Games();
    }
    //Draw outline for selected game
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
    //Draw center category
    Vector2 size = MeasureTextEx(fontBold, categoriesDisplayed[2].name, CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 centerCateg = {CENTER_CATEG_X, CENTER_CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[2].name, centerCateg, CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaCategories_Out));
    

    //Draw left category
    size = MeasureTextEx(fontBold, categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 left1Categ = {LEFT1_CATEG_X, SIDE_CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[1].name, left1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaCategories_Out));
    //Draw right category
    size = MeasureTextEx(fontBold, categoriesDisplayed[3].name, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE);
    Vector2 right1Categ = {RIGHT1_CATEG_X, SIDE_CATEG_Y};
    DrawTextEx(fontBold, categoriesDisplayed[3].name, right1Categ, SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaCategories_Out));
}

//Function for drawing the animation of scrolling categories
void UI_DrawScroll_Categories() {
    //If scrolling to the right
    if (scrollCategories == SCROLL_RIGHT) {
        //If images close enough to final spot
        if (fabsf(newGamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD) {
            //Update displayed categories
            Categories_ScrollRight();
            //Draw categories normally
            UI_DrawCategories_Normal();
            //Draw games
            UI_DrawGames();
            //Reset alphas
            scrollCategories = SCROLL_NO;
            alphaCategories_Out = 1.0f;
            alphaCategories_In = 0.0f;
            UI_ResetDisplayCoords_Games();
            return;
        }
        //Update x coordinates for new games
        newGamesDisplayed[0]->x = Lerp(newGamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[1]->x = Lerp(newGamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[2]->x = Lerp(newGamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[3]->x = Lerp(newGamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[4]->x = Lerp(newGamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        //Update x coordinates for old games
        gamesDisplayed[1]->x = Lerp(gamesDisplayed[1]->x, LEFT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[2]->x = Lerp(gamesDisplayed[2]->x, LEFT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[3]->x = Lerp(gamesDisplayed[3]->x, CENTER_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[4]->x = Lerp(gamesDisplayed[4]->x, RIGHT1_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
        gamesDisplayed[5]->x = Lerp(gamesDisplayed[5]->x, RIGHT2_SCROLLR_X, SCROLL_CATEG_OUT_SPEED);
    }
    //If scrolling to the left
    else {
        //If images close enough to final spot
        if (fabsf(newGamesDisplayed[2]->x - CENTER_GAME_X) <= SCROLL_CATEG_THRESHOLD) {
            //Update displayed categories
            Categories_ScrollLeft();
            //Draw categories normally
            UI_DrawCategories_Normal();
            //Draw games
            UI_DrawGames();
            scrollCategories = SCROLL_NO;
            //Reset alphas
            alphaCategories_Out = 1.0f;
            alphaCategories_Out = 1.0f;
            alphaCategories_In = 0.0f;
            //Reset values for games
            UI_ResetDisplayCoords_Games();
            return;
        }
        //Update x coordinates for new games
        newGamesDisplayed[0]->x = Lerp(newGamesDisplayed[0]->x, LEFT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[1]->x = Lerp(newGamesDisplayed[1]->x, LEFT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[2]->x = Lerp(newGamesDisplayed[2]->x, CENTER_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[3]->x = Lerp(newGamesDisplayed[3]->x, RIGHT1_GAME_X, SCROLL_CATEG_IN_SPEED);
        newGamesDisplayed[4]->x = Lerp(newGamesDisplayed[4]->x, RIGHT2_GAME_X, SCROLL_CATEG_IN_SPEED);
        //Update x coordinates for old games
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
    //Values for left bumper
    Rectangle bumperL = {
        BUMPER_L_X,
        BUMPER_Y,
        BUMPER_W,
        BUMPER_H
    };
    //Values for right bumper
    Rectangle bumperR = {
        BUMPER_R_X,
        BUMPER_Y,
        BUMPER_W,
        BUMPER_H
    };
    //Draw bumper
    DrawRectangleRounded(bumperL, BUMPER_ROUND, BUMPER_SEGMENTS, LIGHTGRAY);
    DrawRectangleRounded(bumperR, BUMPER_ROUND, BUMPER_SEGMENTS, LIGHTGRAY);
    //Values for bumper text and drawing the text
    Vector2 bumperL_Size = MeasureTextEx(fontBold, BUMPER_L_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperR_Size = MeasureTextEx(fontBold, BUMPER_R_TXT, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE);
    Vector2 bumperR_Text = {BUMPER_R_TXT_X, BUMPER_TXT_Y};
    Vector2 bumperL_Text = {BUMPER_L_TXT_X, BUMPER_TXT_Y};
    DrawTextEx(fontBold, BUMPER_R_TXT, bumperR_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, BLACK);
    DrawTextEx(fontBold, BUMPER_L_TXT, bumperL_Text, BUMPER_TXT_SIZE, BUMPER_TXT_SPACE, BLACK);

    //Draw options button
    DrawCircle(OPTION_BTN_X, OPTION_BTN_Y, LS_RADIUS, WHITE);
    Vector2 start2 = {OPTION_LINE_START, OPTION_LINE2_Y};
    Vector2 end2 = {OPTION_LINE_END, OPTION_LINE2_Y};
    DrawLineEx(start2, end2, OPTION_THICKNESS, BLACK);
    Vector2 start1 = {OPTION_LINE_START, OPTION_LINE1_Y};
    Vector2 end1 = {OPTION_LINE_END, OPTION_LINE1_Y};
    DrawLineEx(start1, end1, OPTION_THICKNESS2, BLACK);
    Vector2 start3 = {OPTION_LINE_START, OPTION_LINE3_Y};
    Vector2 end3 = {OPTION_LINE_END, OPTION_LINE3_Y};
    DrawLineEx(start3, end3, OPTION_THICKNESS2, BLACK);

    Vector2 options_Size = MeasureTextEx(fontBold, OPTIONS_TXT, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE);
    Vector2 options_Text = {OPTIONS_TXT_X, OPTIONS_TXT_Y};
    DrawTextEx(fontRegular, OPTIONS_TXT, options_Text, OPTIONS_TXT_SIZE, OPTIONS_TXT_SPACE, WHITE);

    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(SMALL_LOGO_SIZE, CENTER_X), UI_CenterImg_Y(SMALL_LOGO_SIZE, OPTION_BTN_Y + SCREEN_H / 125.0f), SMALL_LOGO_SIZE, SMALL_LOGO_SIZE};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Function for drawing the select controls at the bottom
void UI_DrawBottom() {

    DrawCircle(LS_LEFT_X + (SCREEN_W / 700.0f), LS_Y, LS_RADIUS, Fade(WHITE, alphaGames));
    Vector2 point1 = {LS_LEFT_X - (LS_TRIANGLE_SIZE / 2), LS_Y};
    Vector2 point2 = {LS_LEFT_X + (LS_TRIANGLE_SIZE / 2), LS_Y + LS_TRIANGLE_SIZE / 2};
    Vector2 point3 = {LS_LEFT_X + (LS_TRIANGLE_SIZE / 2), LS_Y - LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point1, point2, point3, BLACK);

    DrawCircle(LS_RIGHT_X - (SCREEN_W / 700.0f), LS_Y, LS_RADIUS, Fade(WHITE, alphaGames));
    Vector2 point4 = {LS_RIGHT_X + (LS_TRIANGLE_SIZE / 2), LS_Y};
    Vector2 point5 = {LS_RIGHT_X - (LS_TRIANGLE_SIZE / 2), LS_Y + LS_TRIANGLE_SIZE / 2};
    Vector2 point6 = {LS_RIGHT_X - (LS_TRIANGLE_SIZE / 2), LS_Y - LS_TRIANGLE_SIZE / 2};
    DrawTriangle(point6, point5, point4, Fade(BLACK, alphaGames));

    //Values for Text
    Vector2 gameText_Size = MeasureTextEx(fontBold, gamesDisplayed[3]->title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 bottomText_Size = MeasureTextEx(fontBold, BOTTOM_TXT, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 consoleText_Size = MeasureTextEx(fontBold, gamesDisplayed[3]->console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE);
    Vector2 gameText = {BOTTOM_TXT_X, GAME_TXT_Y};
    Vector2 bottomText = {BOTTOM_TXT_X, BOTTOM_TXT_Y};
    Vector2 consoleText = {BOTTOM_TXT_X, CONSOLE_TXT_Y};
    int rectWidth;
    if (gameText_Size.x > bottomText_Size.x && gameText_Size.x > consoleText_Size.x) {
        rectWidth = gameText_Size.x;
    }
    else if (bottomText_Size.x > gameText_Size.x && bottomText_Size.x > consoleText_Size.x) {
        rectWidth = bottomText_Size.x;
    }
    else {
        rectWidth = consoleText_Size.x;
    }
    //Draw section
    Rectangle rect = {
        BOTTOM_RECT_X,
        BOTTOM_RECT_Y,
        BOTTOM_RECT_W,
        BOTTOM_RECT_H
    };

    DrawRectangleRoundedLinesEx(rect, BOTTOM_ROUND, BOTTOM_SEGMENTS, THICKNESS_LINE, Fade(WHITE, 0.7));
    DrawRectangleRounded(rect, BOTTOM_ROUND, BOTTOM_SEGMENTS, Fade(BLACK, 0.7));
    //Draw text
    DrawTextEx(fontBold, gamesDisplayed[3]->title, gameText, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaGames));
    DrawTextEx(fontBold, gamesDisplayed[3]->console, consoleText, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaGames));
    DrawTextEx(fontBold, BOTTOM_TXT, bottomText, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE, Fade(WHITE, alphaGames));
}

//Function for drawing the boot screen
void UI_DrawBootScreen() {
    //Clear background
    ClearBackground(BACKGROUND_CLR);
    //Draw logo
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
    //If not scrolling
    if (scrollCategories == SCROLL_NO) {
        UI_DrawCategories_Normal();
    }
    //If scrolling
    else {
        UI_DrawScroll_Categories();
    }
}

//Function for drawing the main menu
void UI_DrawMainMenu() {
    //Clear background
    ClearBackground(BACKGROUND_CLR);

    Rectangle sourceRect = {0.0f, 0.0f, (float)background.width, (float)background.height};
    Rectangle destRect = {UI_CenterImg_X(SCREEN_W, CENTER_X), UI_CenterImg_Y(SCREEN_W / 3.0f * 2.0f, SCREEN_H / 2), SCREEN_W, SCREEN_W / 3.0f * 2.0f};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(background, sourceRect, destRect, origin, 0.0f, Fade(WHITE, 0.2));

    DrawRectangle(0, 0, SCREEN_W, START_SECTION_Y, Fade(BLACK, 0.7));

    Vector2 startSection1 = {0, START_SECTION_Y};
    Vector2 endSection1 = {SCREEN_W, START_SECTION_Y};
    DrawLineEx(startSection1, endSection1, THICKNESS_LINE, WHITE);
    Vector2 startSection2 = {0, END_SECTION_Y};
    Vector2 endSection2 = {SCREEN_W, END_SECTION_Y};
    DrawLineEx(startSection2, endSection2, THICKNESS_LINE, WHITE);
    
    //Draw top ehading
    UI_DrawHeading();
    //Change alphas
    UI_ChangeAlpha_Games(0.25f, 0.25f);
    UI_ChangeAlpha_Categories(0.1f, 0.05f);
    //Draw categories
    UI_DrawCategories();
    //Draw games
    UI_DrawGames();
    //Draw bumpers
    UI_DrawBumpers();
    //Draw bottom section of screen
    UI_DrawBottom();
}

//Function for drawing the diagnostics screen
void UI_DrawDiagnostics() {
    //Clear background
    ClearBackground(BACKGROUND_CLR);
    Vector2 diagnosticsTitle_Size = MeasureTextEx(fontBold, DIAGNOSTICS_TITLE, DIAGNOSTICS_TITLE_SIZE, BUMPER_TXT_SPACE);
    Vector2 diagnosticsTitle_Text = {DIAGNOSTICS_TITLE_X, DIAGNOSTICS_TITLE_Y};
    DrawTextEx(fontBold, DIAGNOSTICS_TITLE, diagnosticsTitle_Text, DIAGNOSTICS_TITLE_SIZE, BUMPER_TXT_SPACE, WHITE);

    Vector2 diagnosticsResolution_Text = {RESOLUTION_TXT_X, RESOLUTION_TXT_Y};
    DrawTextEx(fontRegular, TextFormat("Resolution:  %d:%d", GetMonitorWidth(0), GetMonitorHeight(0)), diagnosticsResolution_Text, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, WHITE);
    Vector2 diagnosticsFPS_Text = {FPS_TXT_X, FPS_TXT_Y};
    DrawTextEx(fontRegular, TextFormat("FPS:  %d", GetFPS()), diagnosticsFPS_Text, DIAGNOSTICS_SIZE, DIAGNOSTICS_SPACING, WHITE);
    return;
}
