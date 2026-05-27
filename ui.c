#include "ui.h"
#include "raylib.h"
#include "games.h"
#include <string.h>

Texture2D img;
Texture2D btnA;
Texture2D btnX;
char *title;
int title_X, title_Y, title_Size;
int img_X, img_Y, img_W, img_H;

void UI_LoadTextures() {
    btnA = LoadTexture("./assets/AButton.png");
    btnX = LoadTexture("./assets/XButton.png");
}

//Function to center a text to a certain X position
int CenterText(char *text, int fontSize, int position) {
    int width = MeasureText(text, fontSize);
    return (position - (width / 2));
}

void DrawImage() {
    if (img.id > 0) {
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

void DrawGames() {
     img_Y = SIDE2_IMG_Y;
    img_W = SIDE2_IMG_W;
    img_H = SIDE2_IMG_H;
    for (int i = 0; i < MAX_GAMES; i ++) {
        if (strcmp(gameLeft2.title, gameLibrary[i].title) == 0) {
            img = gameLibrary[i].cover;
            img_X = LEFT2_IMG_X;
            DrawImage();
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
        else if (strcmp(gameRight2.title, gameLibrary[i].title) == 0) {
            img = gameLibrary[i].cover;
            img_X = RIGHT2_IMG_X;
            DrawImage();
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
    }

    img_Y = SIDE1_IMG_Y;
    img_W = SIDE1_IMG_W;
    img_H = SIDE1_IMG_H; 
    for (int i = 0; i < MAX_GAMES; i++) {
        if (strcmp(gameLeft1.title, gameLibrary[i].title) == 0) {
            img = gameLibrary[i].cover;
            img_X = LEFT1_IMG_X;
            DrawImage();
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
        else if (strcmp(gameRight1.title, gameLibrary[i].title) == 0) {
            img = gameLibrary[i].cover;
            img_X = RIGHT1_IMG_X;
            DrawImage();
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
        
    }

    img_Y = CENTER_IMG_Y;
    img_W = CENTER_IMG_W;
    img_H = CENTER_IMG_H;
    img_X = CENTER_IMG_X;
    for (int i = 0; i < MAX_GAMES; i++) {
        if (strcmp(gameCenter.title, gameLibrary[i].title) == 0) {
            img = gameLibrary[i].cover;
            DrawImage();
            DrawRectangleLinesEx(
                (Rectangle){
                    img_X - THICKNESS_SELECT,
                    img_Y - THICKNESS_SELECT,
                    img_W + (2 * THICKNESS_SELECT),
                    img_H + (2 * THICKNESS_SELECT)
                },
                THICKNESS_SELECT, 
                BLUE
            );
        }
    }
}

void DrawSelect() {
    DrawText(SELECT_TEXT1, SELECT_TEXT1_X, SELECT_Y, SELECT_TEXT_SIZE, BLUE);
    img = btnA;
    img_X = A_BTN_X;
    img_Y = SELECT_Y;
    img_W = BTN_SIZE;
    img_H = BTN_SIZE;
    DrawImage();
    DrawImage(SELECT_TEXT2, SELECT_TEXT2_X, SELECT_Y, SELECT_TEXT_SIZE, BLUE);
    img = btnX;
    img_X = X_BTN_X;
    img_Y = SELECT_Y;
    img_W = BTN_SIZE;
    img_H = BTN_SIZE;
    DrawImage();
    DrawText(SELECT_TEXT3, SELECT_TEXT3_X, SELECT_Y, SELECT_TEXT_SIZE, BLUE);
}

//Draw the boot screen
void UI_DrawBootScreen(void) {
    ClearBackground(BACKGROUND_CLR);
    DrawCircle(400, 225, 200, RED);
    DrawText("Welcome", 250, 185, 75, BLUE);
}

//Draw the main menu
void UI_DrawMainMenu(int currentSelection) {
    ClearBackground(BACKGROUND_CLR);
    DrawText(COMPANY_NAME, CenterText(COMPANY_NAME, COMPANY_TEXT_SIZE, CENTER), COMPANY_Y, COMPANY_TEXT_SIZE, BLUE);
    DrawText(PICK_GAME_TEXT, CenterText(PICK_GAME_TEXT, PICK_GAME_TEXT_SIZE, CENTER), PICK_GAME_Y, PICK_GAME_TEXT_SIZE, BLUE);

    DrawGames();
   
    DrawSelect()
}

void UI_DrawDiagnostics(void) {
    return;
}