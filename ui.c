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

Texture2D img;
Texture2D btnA;
Texture2D btnX;
char *title;
int title_X, title_Y, title_Size;
int img_X, img_Y, img_W, img_H;

enum Scroll scrollGames = SCROLL_NO;
enum Scroll scrollCategories = SCROLL_NO;

float alpha;


//Change the alpha value for fading the display
void UI_ChangeAlpa(float offRate, float onRate) {
    if (scrollGames == SCROLL_NO) {
        if (alpha <= 1.0f) {
            alpha += onRate;
        } 
        else {
            alpha = 1.0f;
        }
    }
    else {
        if (alpha <= 0.0f) {
            alpha = 0.0f;
        }
        else {
            alpha -= offRate;
        }
    }
}

//Reset the coordinates for the displayed games
void UI_ResetDisplayCoords_Games() {

    gamesDisplayed[0].x = LEFT3_GAME_X;
    gamesDisplayed[1].x = LEFT2_GAME_X;
    gamesDisplayed[2].x = LEFT1_GAME_X;
    gamesDisplayed[3].x = CENTER_GAME_X;
    gamesDisplayed[4].x = RIGHT1_GAME_X;
    gamesDisplayed[5].x = RIGHT2_GAME_X;
    gamesDisplayed[6].x = RIGHT3_GAME_X;

    gamesDisplayed[0].y = SIDE3_GAME_Y;
    gamesDisplayed[1].y = SIDE2_GAME_Y;
    gamesDisplayed[2].y = SIDE1_GAME_Y;
    gamesDisplayed[3].y = CENTER_GAME_Y;
    gamesDisplayed[4].y = SIDE1_GAME_Y;
    gamesDisplayed[5].y = SIDE2_GAME_Y;
    gamesDisplayed[6].y = SIDE3_GAME_Y;

    gamesDisplayed[0].w = SIDE3_GAME_W;
    gamesDisplayed[1].w = SIDE2_GAME_W;
    gamesDisplayed[2].w = SIDE1_GAME_W;
    gamesDisplayed[3].w = CENTER_GAME_W;
    gamesDisplayed[4].w = SIDE1_GAME_W;
    gamesDisplayed[5].w = SIDE2_GAME_W;
    gamesDisplayed[6].w = SIDE3_GAME_W;

    gamesDisplayed[0].h = SIDE3_GAME_H;
    gamesDisplayed[1].h = SIDE2_GAME_H;
    gamesDisplayed[2].h = SIDE1_GAME_H;
    gamesDisplayed[3].h = CENTER_GAME_H;
    gamesDisplayed[4].h = SIDE1_GAME_H;
    gamesDisplayed[5].h = SIDE2_GAME_H;
    gamesDisplayed[6].h = SIDE3_GAME_H;
 
}

//Function for drawing an arrow button
void UI_DrawArrow(int xPos, int yPos, int direction) {
    int radius = BTN_RADIUS;

    Vector2 centerA = { xPos, yPos };
    DrawCircleV(centerA, radius, WHITE);

    if (direction == LEFT) {
        Vector2 v1 = {xPos + (2.0f / 3) * (radius), yPos};
        Vector2 v2 = {xPos - (1.0f / 4) * (radius), yPos};
        DrawLineEx(v1, v2, 4.0f, GRAY);
    
        Vector2 triangleTip   = { xPos - (3.0f / 4) * (radius), yPos };
        Vector2 bottomRight   = { xPos - (1.0f / 4) * (radius), yPos + (1.0f / 2) * (radius) };
        Vector2 topRight      = { xPos - (1.0f / 4) * (radius), yPos - (1.0f / 2) * (radius) };
        DrawTriangle(triangleTip, bottomRight, topRight, GRAY);
    }
    else if (direction == RIGHT) {
        Vector2 v1 = {xPos - (2.0f / 3) * (radius), yPos};
        Vector2 v2 = {xPos + (1.0f / 4) * (radius), yPos};

        DrawLineEx(v1, v2, 4.0f, GRAY);
    
        Vector2 triangleTip   = { xPos + (3.0f / 4) * (radius), yPos };
        Vector2 bottomRight   = { xPos + (1.0f / 4) * (radius), yPos - (1.0f / 2) * (radius) };
        Vector2 topRight      = { xPos + (1.0f / 4) * (radius), yPos + (1.0f / 2) * (radius) };
        DrawTriangle(triangleTip, bottomRight, topRight, GRAY);
    }
}

//Function for drawing an image
void UI_DrawImage() {
    if (1) {
            Rectangle sourceRect = {0.0f, 0.0f, (float)img.width, (float)img.height};
            Rectangle destRect = {(float)img_X, img_Y, img_W, img_H};
            Vector2 origin = {0.0f, 0.0f};
            DrawTexturePro(img, sourceRect, destRect, origin, 0.0f, WHITE);
        } 
        else {
            // Draw a gray placeholder box if the image file is missing!
            DrawRectangle(img_X, img_Y, img_W, img_H, LIGHTGRAY);
            DrawRectangleLines(img_X, img_Y, img_W, img_H, GRAY);
        }
}

//Function for drawing a game cover
void UI_DrawGame(int i) {
    img_Y = gamesDisplayed[i].y;
    img_W = gamesDisplayed[i].w;
    img_H = gamesDisplayed[i].h;
    img = gamesDisplayed[i].cover;
    img_X = gamesDisplayed[i].x;

    UI_DrawImage();
    DrawRectangleLinesEx(
        (Rectangle){
            img_X - THICKNESS_OTHER,
            img_Y - THICKNESS_OTHER,
            img_W + (2 * THICKNESS_OTHER),
            img_H + (2 * THICKNESS_OTHER)
        },
        THICKNESS_OTHER, 
        GRAY
    );
}

//Function for drawing the games when scrolling to the left
void UI_DrawGames_Left() {
    UI_DrawGame(6);
    UI_DrawGame(1);
    UI_DrawGame(5);
    UI_DrawGame(2);
    UI_DrawGame(3);
    UI_DrawGame(4);
}

//Function for drawing the games when scrolling to the right
void UI_DrawGames_Right() {
    UI_DrawGame(0);
    UI_DrawGame(1);
    UI_DrawGame(5);
    UI_DrawGame(4);
    UI_DrawGame(3);
    UI_DrawGame(2);
}

//Function for drawing the animation of scrolling games
void UI_DrawScroll_Games() {

    if (scrollGames == SCROLL_LEFT) {
        UI_DrawGames_Left();
        if (fabsf(gamesDisplayed[4].x - CENTER_GAME_X) <= SCROLL_THRESHOLD) {
            scrollGames = SCROLL_NO;
            Games_ScrollLeft();
            UI_ResetDisplayCoords_Games();
            return;
        }

        gamesDisplayed[1].x = Lerp(gamesDisplayed[1].x, LEFT3_GAME_X, 0.075f);
        gamesDisplayed[2].x = Lerp(gamesDisplayed[2].x, LEFT2_GAME_X, 0.1f);
        gamesDisplayed[3].x = Lerp(gamesDisplayed[3].x, LEFT1_GAME_X, 0.1f);
        gamesDisplayed[4].x = Lerp(gamesDisplayed[4].x, CENTER_GAME_X, 0.1f);
        gamesDisplayed[5].x = Lerp(gamesDisplayed[5].x, RIGHT1_GAME_X, 0.1f); 
        gamesDisplayed[6].x = Lerp(gamesDisplayed[6].x, RIGHT2_GAME_X, 0.1f); 

        gamesDisplayed[1].y = Lerp(gamesDisplayed[1].y, SIDE3_GAME_Y, 0.075f);
        gamesDisplayed[2].y = Lerp(gamesDisplayed[2].y, SIDE2_GAME_Y, 0.1f);
        gamesDisplayed[3].y = Lerp(gamesDisplayed[3].y, SIDE1_GAME_Y, 0.1f);
        gamesDisplayed[4].y = Lerp(gamesDisplayed[4].y, CENTER_GAME_Y, 0.1f);
        gamesDisplayed[5].y = Lerp(gamesDisplayed[5].y, SIDE1_GAME_Y, 0.1f);
        gamesDisplayed[6].y = Lerp(gamesDisplayed[6].y, SIDE2_GAME_Y, 0.1f);   

        gamesDisplayed[1].w = Lerp(gamesDisplayed[1].w, SIDE3_GAME_W, 0.075f);
        gamesDisplayed[2].w = Lerp(gamesDisplayed[2].w, SIDE2_GAME_W, 0.1f);
        gamesDisplayed[3].w = Lerp(gamesDisplayed[3].w, SIDE1_GAME_W, 0.1f);
        gamesDisplayed[4].w = Lerp(gamesDisplayed[4].w, CENTER_GAME_W, 0.1f);
        gamesDisplayed[5].w = Lerp(gamesDisplayed[5].w, SIDE1_GAME_W, 0.1f); 
        gamesDisplayed[6].w = Lerp(gamesDisplayed[6].w, SIDE2_GAME_W, 0.1f);

        gamesDisplayed[1].h = Lerp(gamesDisplayed[1].h, SIDE3_GAME_W, 0.075f);
        gamesDisplayed[2].h = Lerp(gamesDisplayed[2].h, SIDE2_GAME_H, 0.1f);
        gamesDisplayed[3].h = Lerp(gamesDisplayed[3].h, SIDE1_GAME_H, 0.1f);
        gamesDisplayed[4].h = Lerp(gamesDisplayed[4].h, CENTER_GAME_H, 0.1f);
        gamesDisplayed[5].h = Lerp(gamesDisplayed[5].h, SIDE1_GAME_H, 0.1f);  
        gamesDisplayed[6].h = Lerp(gamesDisplayed[6].h, SIDE2_GAME_H, 0.1f);
    }
    
    else if (scrollGames == SCROLL_RIGHT) {
        UI_DrawGames_Right();
        if (fabsf(gamesDisplayed[2].x - CENTER_GAME_X) <= SCROLL_THRESHOLD) {
            scrollGames = SCROLL_NO;
            Games_ScrollRight();
            UI_ResetDisplayCoords_Games();
            return;
        }
        
        gamesDisplayed[0].x = Lerp(gamesDisplayed[0].x, LEFT2_GAME_X, 0.1f);
        gamesDisplayed[1].x = Lerp(gamesDisplayed[1].x, LEFT1_GAME_X, 0.1f);
        gamesDisplayed[2].x = Lerp(gamesDisplayed[2].x, CENTER_GAME_X, 0.1f);
        gamesDisplayed[3].x = Lerp(gamesDisplayed[3].x, RIGHT1_GAME_X, 0.1f);
        gamesDisplayed[4].x = Lerp(gamesDisplayed[4].x, RIGHT2_GAME_X, 0.1f);
        gamesDisplayed[5].x = Lerp(gamesDisplayed[5].x, RIGHT3_GAME_X, 0.075f); 

        gamesDisplayed[0].y = Lerp(gamesDisplayed[0].y, SIDE2_GAME_Y, 0.1f);
        gamesDisplayed[1].y = Lerp(gamesDisplayed[1].y, SIDE1_GAME_Y, 0.1f);
        gamesDisplayed[2].y = Lerp(gamesDisplayed[2].y, CENTER_GAME_Y, 0.1f);
        gamesDisplayed[3].y = Lerp(gamesDisplayed[3].y, SIDE1_GAME_Y, 0.1f);
        gamesDisplayed[4].y = Lerp(gamesDisplayed[4].y, SIDE2_GAME_Y, 0.1f); 
        gamesDisplayed[5].y = Lerp(gamesDisplayed[5].y, SIDE3_GAME_Y, 0.075f); 

        gamesDisplayed[0].w = Lerp(gamesDisplayed[0].w, SIDE2_GAME_W, 0.1f);
        gamesDisplayed[1].w = Lerp(gamesDisplayed[1].w, SIDE1_GAME_W, 0.1f);
        gamesDisplayed[2].w = Lerp(gamesDisplayed[2].w, CENTER_GAME_W, 0.1f);
        gamesDisplayed[3].w = Lerp(gamesDisplayed[3].w, SIDE1_GAME_W, 0.1f);
        gamesDisplayed[4].w = Lerp(gamesDisplayed[4].w, SIDE2_GAME_W, 0.1f);
        gamesDisplayed[5].w = Lerp(gamesDisplayed[5].w, SIDE3_GAME_W, 0.075f);

        gamesDisplayed[0].h = Lerp(gamesDisplayed[0].h, SIDE2_GAME_H, 0.1f);
        gamesDisplayed[1].h = Lerp(gamesDisplayed[1].h, SIDE1_GAME_H, 0.1f);
        gamesDisplayed[2].h = Lerp(gamesDisplayed[2].h, CENTER_GAME_H, 0.1f);
        gamesDisplayed[3].h = Lerp(gamesDisplayed[3].h, SIDE1_GAME_H, 0.1f);
        gamesDisplayed[4].h = Lerp(gamesDisplayed[4].h, SIDE2_GAME_H, 0.1f);
        gamesDisplayed[5].h = Lerp(gamesDisplayed[5].h, SIDE3_GAME_H, 0.075f);
    }
}

//Function for drawing the cateogires when they are static
void UI_DrawCategories_Normal() {
    Font font = GetFontDefault();
    float spacing = CENTER_CATEG_SIZE / 10;
    Vector2 size = MeasureTextEx(font, categoriesDisplayed[2].name, CENTER_CATEG_SIZE, spacing);
    //char categ_header[30];
    printf("name: %s\n", categoriesDisplayed[2].name);
    //sprintf(categ_header, "[%s]", categories[2].name);
    //printf("draw: %s\n", categ_header);
    DrawText(categoriesDisplayed[2].name, CENTER_CATEG_X, CATEG_Y, CENTER_CATEG_SIZE, BLUE);

    spacing = SIDE1_CATEG_SIZE / 10;
    size = MeasureTextEx(font, categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, spacing);
    //sprintf(categ_header, "[%s]", categories[1].name);
    DrawText(categoriesDisplayed[1].name, LEFT1_CATEG_X, CATEG_Y, SIDE1_CATEG_SIZE, BLUE);

    size = MeasureTextEx(font, categoriesDisplayed[3].name, SIDE1_CATEG_SIZE, spacing);
    //sprintf(categ_header, "[%s]", categories[3].name);
    DrawText(categoriesDisplayed[3].name, RIGHT1_CATEG_X, CATEG_Y, SIDE1_CATEG_SIZE, BLUE);
}

//Function for drawing the animation of scrolling categories
void UI_DrawScroll_Categories() {
    if (scrollCategories == SCROLL_RIGHT) {
        Categories_ScrollRight();
        UI_DrawCategories_Normal();
        scrollCategories = SCROLL_NO;
        UI_ResetDisplayCoords_Games();
    }
    else {
        Categories_ScrollLeft();
        UI_DrawCategories_Normal();
        scrollCategories = SCROLL_NO;
        UI_ResetDisplayCoords_Games();
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

//Function for drawing the controls at the top left corner
void UI_DrawCtrls_L() {
    UI_DrawArrow(L_ARROW_X, ARROW_Y, LEFT);
    UI_DrawArrow(R_ARROW_X, ARROW_Y, RIGHT);
    DrawText(ARROW_TXT, ARROW_TXT_X, ARROW_TXT_Y, BTN_TXT_SIZE, BLUE);  
}

//Function for drawing the controls at the top right corner
void UI_DrawCtrls_R() {

    DrawText (BTN_BACK_TXT, BTN_BACK_TXT_X, BTN_BACK_TXT_Y, BTN_TXT_SIZE, BLUE);

    Vector2 centerB = { XBOX_B_X, BTN_BACK_Y };
    DrawCircleV(centerB, BTN_RADIUS, RED);
    DrawText("B", XBOX_B_TXT_X, XBOX_B_TXT_Y, BTN_TXT_SIZE, WHITE);

    DrawText ("/", BTN_BACK_SLASH_X, XBOX_B_TXT_Y, BTN_TXT_SIZE, BLUE);

    Vector2 centerO = { PS_O_X, BTN_BACK_Y };
    DrawCircleV(centerO, BTN_RADIUS, BLACK);
    DrawText("O", PS_O_TXT_X, PS_O_TXT_Y, BTN_TXT_SIZE, RED);

}

//Function for drawing the select controls at the bottom
void UI_DrawBottom() {
    DrawRectangle(0, BOTTOM_Y, SCREEN_W, (SCREEN_H - BOTTOM_Y), WHITE);
    DrawLine(0, BOTTOM_Y, SCREEN_W, BOTTOM_Y, BLUE);
    
    DrawText(GAME_TXT, BOTTOM_TXT_X, GAME_TXT_Y, BTN_TXT_SIZE, Fade(BLUE, alpha));

    DrawText(BOTTOM_TXT, BOTTOM_TXT_X, BOTTOM_TXT_Y, BTN_TXT_SIZE, Fade(BLUE, alpha));
}

//Function for drawing the boot screen
void UI_DrawBootScreen() {
    ClearBackground(BACKGROUND_CLR);
    /*DrawCircle(400, 225, 200, RED);
    DrawText("Welcome", 250, 185, 75, BLUE);*/
    Texture2D spiderLogo = LoadTexture("./assets/SpiderLogo.png");

    Rectangle sourceRect = {0.0f, 0.0f, (float)spiderLogo.width, (float)spiderLogo.height};
    Rectangle destRect = {UI_CenterImg_X(500, CENTER_X), UI_CenterImg_Y(500, SCREEN_H / 2), 500, 500};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(spiderLogo, sourceRect, destRect, origin, 0.0f, WHITE);
}

//Function for drawing the heading at the top of the screen
void UI_DrawHeading() {
    //DrawText(COMPANY_NAME, UI_CenterText_X(COMPANY_NAME, COMPANY_TXT_SIZE, CENTER_X), COMPANY_Y, COMPANY_TXT_SIZE, BLUE);
    //DrawText(PICK_GAME_TXT, UI_CenterText_X(PICK_GAME_TXT, PICK_GAME_TXT_SIZE, CENTER_X), PICK_GAME_Y, PICK_GAME_TXT_SIZE, BLUE);
    DrawRectangle(0, 0, SCREEN_W, TOP_Y, WHITE);
    DrawLine(0, TOP_Y, SCREEN_W, TOP_Y, BLUE);
}

//Function for drawing games depending on whether it is scrolling
void UI_DrawGames() {
    if (scrollGames == SCROLL_NO) {
        UI_DrawGame(1);
        UI_DrawGame(5);
        UI_DrawGame(2);
        UI_DrawGame(4); 
        UI_DrawGame(3);
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
        Fade(BLUE, alpha)
    );
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
    UI_ChangeAlpa(0.25f, 0.25f);
    UI_DrawGames();

    UI_DrawCategories();
    UI_DrawBottom();
    //UI_DrawCtrls_L();
    //UI_DrawCtrls_R();
}

//Function for drawing the diagnostics screen
void UI_DrawDiagnostics() {
    return;
}
