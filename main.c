#include "raylib.h"
#include "states.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "categories.h"
#include <stdio.h>
#include <math.h>



//Main function
int main(void)
{
    //Synch refresh rates
    SetConfigFlags(FLAG_VSYNC_HINT);
    //Get width and height of screen
    int monitorWidth = GetMonitorWidth(0);
    int monitorHeight = GetMonitorHeight(0);
    //Initialize a window 
    InitWindow(monitorWidth, monitorHeight, "Custom Console OS");
    //Go full screen
    ToggleFullscreen();
    //Load fonts
    fontRegular = LoadFont("assets/fonts/Exo2-Regular.ttf");
    fontBold = LoadFont("assets/fonts/Exo2-Bold.ttf");
    //Initialize state machine
    Games_Init();
    Categories_Init();
    State_Init();
    //Setup render
    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);
    //Keep display sharp when stretched
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    //Set target FPS
    SetTargetFPS(FPS);
    //Main game loop
    while (!WindowShouldClose()) {
        //Virtual window
        BeginTextureMode(target);
        //Clear background
        ClearBackground(WHITE);
        //Update console and draw
        State_UpdateAndDraw();
        //Finish virtual window
        EndTextureMode();
        
        //Start drawing
        BeginDrawing();
        //Clear background
        ClearBackground(BLACK);
        //Calculate scaling so the game stays perfectly shaped
        float scale = fminf((float)GetScreenWidth() / SCREEN_W, 
                        (float)GetScreenHeight() / SCREEN_H);
        //Calculate centered position
        Rectangle destRec = {
            (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f,
            (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f,
            SCREEN_W * scale,
            SCREEN_H * scale
        };
        //Draw the scaled texture
        DrawTexturePro(target.texture, 
            (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, 
            destRec, 
            (Vector2){ 0, 0 }, 0.0f, WHITE);
        //Stop drawing
        EndDrawing();
    }
    //Unload texture
    UnloadRenderTexture(target);
    //Unload game textures and close window
    Games_UnloadTextures();
    //Close window
    CloseWindow();
    return 0;
}