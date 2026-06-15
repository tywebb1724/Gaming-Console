#include "raylib.h"
#include "states.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "categories.h"
#include <stdio.h>
#include <math.h>

int monitorWidth;
int monitorHeight;


//Main function
int main(void)
{
    //Synch refresh rates
    SetConfigFlags(FLAG_VSYNC_HINT);
    //Initialize a window 
    InitWindow(1280, 720, "Custom Console OS");
    ToggleFullscreen();
    
    //Load fonts
    fontRegular = LoadFont("assets/fonts/Exo2-Regular.ttf");
    fontBold = LoadFont("assets/fonts/Exo2-Bold.ttf");

    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);
    //Initialize state machine
    Games_Init();
    Categories_Init();
    State_Init();

    SetTargetFPS(0);
    //Main game loop
    while (!WindowShouldClose()) {
        //Start drawing
        BeginDrawing();
        //Update console and draw
        State_UpdateAndDraw();

        DrawText(TextFormat("FPS:%d", GetFPS()), 100, 100, 50, RED);        
        //Stop drawing
        EndDrawing();
    }
    //Unload texture
    //UnloadRenderTexture(target);
    //Unload game textures and close window
    Games_UnloadTextures();
    //Close window
    CloseWindow();
    return 0;
}