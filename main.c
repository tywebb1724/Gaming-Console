#include "raylib.h"
#include "states.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "categories.h"
#include <stdio.h>

//Main function
int main(void)
{
    //Initialize a window and the target FPS
    InitWindow(SCREEN_W, SCREEN_H, "Custom Console OS");
    SetTargetFPS(FPS);
    
    //Initialize state machine
    Games_Init();
    Categories_Init();
    State_Init();
    //Main game loop
    while (!WindowShouldClose()) {
        //Start drawing
        BeginDrawing();
        //Update console and draw
        State_UpdateAndDraw();
        //Stop drawing
        EndDrawing();
    }
    //Unload game textures and close window
    Games_UnloadTextures();
    CloseWindow();
    return 0;
}