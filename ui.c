#include "ui.h"
#include "raylib.h"
#include "raymath.h"
#include "games.h"
#include <string.h>
#include <math.h>
#include "states.h"
#include "consoles.h"
#include "ui_config.h"
#include <stdio.h>

Texture2D img;
Texture2D btnA;
Texture2D btnX;
char *title;
int title_X, title_Y, title_Size;
int img_X, img_Y, img_W, img_H;

enum Scroll scroll = SCROLL_NO;

float alpha;

//Change the alpha value for fading the display
void UI_ChangeAlpa(float offRate, float onRate) {
    if (scroll == SCROLL_NO) {
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

//Reset the coordinates for the displayed consoles
void UI_ResetDisplayCoords_Consoles() {
    
    consolesDisplayed[0].x = LEFT2_CONS_X;
    consolesDisplayed[1].x = LEFT1_CONS_X;
    consolesDisplayed[2].x = CENTER_CONS_X;
    consolesDisplayed[3].x = RIGHT1_CONS_X;
    consolesDisplayed[4].x = RIGHT2_CONS_X;

    consolesDisplayed[0].y = SIDE2_CONS_Y;
    consolesDisplayed[1].y = SIDE1_CONS_Y;
    consolesDisplayed[2].y = CENTER_CONS_Y;
    consolesDisplayed[3].y = SIDE1_CONS_Y;
    consolesDisplayed[4].y = SIDE2_CONS_Y;

    consolesDisplayed[0].w = SIDE2_CONS_W;
    consolesDisplayed[1].w = SIDE1_CONS_W;
    consolesDisplayed[2].w = CENTER_CONS_W;
    consolesDisplayed[3].w = SIDE1_CONS_W;
    consolesDisplayed[4].w = SIDE2_CONS_W;

    consolesDisplayed[0].h = SIDE2_CONS_H;
    consolesDisplayed[1].h = SIDE1_CONS_H;
    consolesDisplayed[2].h = CENTER_CONS_H;
    consolesDisplayed[3].h = SIDE1_CONS_H;
    consolesDisplayed[4].h = SIDE2_CONS_H;

    return;
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
    img = gameLibrary[gamesDisplayed[i].index].cover;
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

//Function for drawing the games when in a static position
void UI_DrawGamesNormal() {
    UI_DrawGame(1);
    UI_DrawGame(5);
    UI_DrawGame(2);
    UI_DrawGame(4); 
    UI_DrawGame(3);
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

    if (scroll == SCROLL_LEFT) {
        UI_DrawGames_Left();
        if (fabsf(gamesDisplayed[4].x - CENTER_GAME_X) <= SCROLL_THRESHOLD) {
            scroll = SCROLL_NO;
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
    
    else if (scroll == SCROLL_RIGHT) {
        UI_DrawGames_Right();
        if (fabsf(gamesDisplayed[2].x - CENTER_GAME_X) <= SCROLL_THRESHOLD) {
            scroll = SCROLL_NO;
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

//Function for drawing a console cover
void UI_DrawConsole(int i) {
    img_Y = consolesDisplayed[i].y;
    img_W = consolesDisplayed[i].w;
    img_H = consolesDisplayed[i].h;
    img = consoleLibrary[consolesDisplayed[i].index].cover;
    img_X = consolesDisplayed[i].x;
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

//Function for drawing the consoles when in a static position
void UI_DrawConsoles_Normal() {
    UI_DrawConsole(1);
    UI_DrawConsole(3);
    UI_DrawConsole(2);
}

//Function for drawing the consoles when scrolling to the left
void UI_DrawConsoles_Left() {
    UI_DrawConsole(1);
    UI_DrawConsole(4);
    UI_DrawConsole(2);
    UI_DrawConsole(3);
}

//Function for drawing the consoles when scrolling to the right
void UI_DrawConsoles_Right() {
    UI_DrawConsole(3);
    UI_DrawConsole(0);
    UI_DrawConsole(2);
    UI_DrawConsole(1);
}

//Function for drawing the animation of scrolling consoles
void UI_DrawScroll_Consoles() {
    if (scroll == SCROLL_LEFT) {
        UI_DrawConsoles_Left();
        if (fabsf(consolesDisplayed[3].x - CENTER_CONS_X) <= SCROLL_THRESHOLD) {
            scroll = SCROLL_NO;
            Consoles_ScrollLeft();
            UI_ResetDisplayCoords_Consoles();
            return;
        }

        consolesDisplayed[1].x = Lerp(consolesDisplayed[1].x, LEFT2_CONS_X, 0.075f);
        consolesDisplayed[2].x = Lerp(consolesDisplayed[2].x, LEFT1_CONS_X, 0.1f);
        consolesDisplayed[3].x = Lerp(consolesDisplayed[3].x, CENTER_CONS_X, 0.1f);
        consolesDisplayed[4].x = Lerp(consolesDisplayed[4].x, RIGHT1_CONS_X, 0.1f);

        consolesDisplayed[1].y = Lerp(consolesDisplayed[1].y, SIDE2_CONS_Y, 0.075f);
        consolesDisplayed[2].y = Lerp(consolesDisplayed[2].y, SIDE1_CONS_Y, 0.1f);
        consolesDisplayed[3].y = Lerp(consolesDisplayed[3].y, CENTER_CONS_Y, 0.1f);
        consolesDisplayed[4].y = Lerp(consolesDisplayed[4].y, SIDE1_CONS_Y, 0.1f);  

        consolesDisplayed[1].w = Lerp(consolesDisplayed[1].w, SIDE2_CONS_W, 0.075f);
        consolesDisplayed[2].w = Lerp(consolesDisplayed[2].w, SIDE1_CONS_W, 0.1f);
        consolesDisplayed[3].w = Lerp(consolesDisplayed[3].w, CENTER_CONS_W, 0.1f);
        consolesDisplayed[4].w = Lerp(consolesDisplayed[4].w, SIDE1_CONS_W, 0.1f);

        consolesDisplayed[1].h = Lerp(consolesDisplayed[1].h, SIDE2_CONS_H, 0.075f);
        consolesDisplayed[2].h = Lerp(consolesDisplayed[2].h, SIDE1_CONS_H, 0.1f);
        consolesDisplayed[3].h = Lerp(consolesDisplayed[3].h, CENTER_CONS_H, 0.1f);
        consolesDisplayed[4].h = Lerp(consolesDisplayed[4].h, SIDE1_CONS_H, 0.1f);
    }
    
    else if (scroll == SCROLL_RIGHT) {
        UI_DrawConsoles_Right();
        if (fabsf(consolesDisplayed[1].x - CENTER_CONS_X) <= SCROLL_THRESHOLD) {
            scroll = SCROLL_NO;
            Consoles_ScrollRight();
            UI_ResetDisplayCoords_Consoles();
            return;
        }
        
        consolesDisplayed[0].x = Lerp(consolesDisplayed[0].x, LEFT1_CONS_X, 0.1f);
        consolesDisplayed[1].x = Lerp(consolesDisplayed[1].x, CENTER_CONS_X, 0.1f);
        consolesDisplayed[2].x = Lerp(consolesDisplayed[2].x, RIGHT1_CONS_X, 0.1f);
        consolesDisplayed[3].x = Lerp(consolesDisplayed[3].x, RIGHT2_CONS_X, 0.075f);


        consolesDisplayed[0].y = Lerp(consolesDisplayed[0].y, SIDE1_CONS_Y, 0.1f);
        consolesDisplayed[1].y = Lerp(consolesDisplayed[1].y, CENTER_CONS_Y, 0.1f);
        consolesDisplayed[2].y = Lerp(consolesDisplayed[2].y, SIDE1_CONS_Y, 0.1f);
        consolesDisplayed[3].y = Lerp(consolesDisplayed[3].y, SIDE2_CONS_Y, 0.075f);

        consolesDisplayed[0].w = Lerp(consolesDisplayed[0].w, SIDE1_CONS_W, 0.1f);
        consolesDisplayed[1].w = Lerp(consolesDisplayed[1].w, CENTER_CONS_W, 0.1f);
        consolesDisplayed[2].w = Lerp(consolesDisplayed[2].w, SIDE1_CONS_W, 0.1f);
        consolesDisplayed[3].w = Lerp(consolesDisplayed[3].w, SIDE2_CONS_W, 0.075f);

        consolesDisplayed[0].h = Lerp(consolesDisplayed[0].h, SIDE1_CONS_H, 0.1f);
        consolesDisplayed[1].h = Lerp(consolesDisplayed[1].h, CENTER_CONS_H, 0.1f);
        consolesDisplayed[2].h = Lerp(consolesDisplayed[2].h, SIDE1_CONS_H, 0.1f);
        consolesDisplayed[3].h = Lerp(consolesDisplayed[3].h, SIDE2_CONS_H, 0.075f);
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
void UI_DrawSelect() {
    //"PRESS"
    DrawText(SELECT_TXT1, SELECT_TXT1_X, SELECT_TXT_Y, BTN_TXT_SIZE, Fade(BLUE, alpha));
    //A button
    Vector2 centerA = { XBOX_A_X, XBOX_A_Y };
    DrawCircleV(centerA, BTN_RADIUS, Fade(DARKGREEN, alpha));
    DrawText("A", XBOX_A_TXT_X, SELECT_TXT_Y, BTN_TXT_SIZE, Fade(WHITE, alpha));
    //"/"
    DrawText(SELECT_TXT2, SELECT_TXT2_X, SELECT_TXT_Y, BTN_TXT_SIZE, Fade(BLUE, alpha));
    //X button
    Vector2 centerX = { PS_X_X, PS_X_Y };
    DrawCircleV(centerX, BTN_RADIUS, Fade(BLUE, alpha));
    DrawText("X", PS_X_TXT_X, SELECT_TXT_Y, BTN_TXT_SIZE, Fade(BLACK, alpha));
    //"TO PLAY"
    DrawText(SELECT_TXT3, SELECT_TXT3_X, SELECT_TXT_Y, BTN_TXT_SIZE, Fade(BLUE, alpha));

    DrawRectangleLinesEx(
                (Rectangle){
                    SELECT_TXT1_X - 2 * SELECT_PADDING_X,
                    SELECT_TXT_Y - SELECT_PADDING_Y / 2,
                    SELECT_LEN + 4 * SELECT_PADDING_X,
                    BTN_TXT_SIZE + SELECT_PADDING_Y
                },
                THICKNESS_SELECT_TXT, 
                Fade(BLUE, alpha)
            );
}

//Function for drawing the boot screen
void UI_DrawBootScreen() {
    ClearBackground(BACKGROUND_CLR);
    DrawCircle(400, 225, 200, RED);
    DrawText("Welcome", 250, 185, 75, BLUE);
}

//Function for drawing the heading at the top of the screen
void UI_DrawHeading() {
    DrawText(COMPANY_NAME, UI_CenterText_X(COMPANY_NAME, COMPANY_TXT_SIZE, CENTER_X), COMPANY_Y, COMPANY_TXT_SIZE, BLUE);
    DrawText(PICK_GAME_TXT, UI_CenterText_X(PICK_GAME_TXT, PICK_GAME_TXT_SIZE, CENTER_X), PICK_GAME_Y, PICK_GAME_TXT_SIZE, BLUE);
}

//Function for drawing games depending on whether it is scrolling
void UI_DrawGames() {
    if (scroll == SCROLL_NO) {
        UI_DrawGamesNormal();
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

//Function for drawing consoles depending on whether it is scrolling
void UI_DrawConsoles() {
    if (scroll == SCROLL_NO) {
        UI_DrawConsoles_Normal();
    }
    else {
        UI_DrawScroll_Consoles();
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

//Function for drawing the main menu
void UI_DrawMainMenu() {
    ClearBackground(BACKGROUND_CLR);
    UI_DrawHeading();
    UI_ChangeAlpa(0.25f, 0.25f);

    switch (currentMenuState) {
        case (CONSOLES):
        UI_DrawConsoles();
        break;

        case (GAMES):
        UI_DrawGames();
        break;

    }

    UI_DrawSelect();
    UI_DrawCtrls_L();
    UI_DrawCtrls_R();
}

//Function for drawing the diagnostics screen
void UI_DrawDiagnostics() {
    return;
}