#include "raylib.h"
#include "states.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "consoles.h"
#include <stdio.h>


int main(void)
{
    // Initialize a window 800 pixels wide by 450 pixels high
    InitWindow(SCREEN_W, SCREEN_H, "Custom Console OS");
    SetTargetFPS(FPS);
    
    //Initialize state machine
    Games_Init();
    Consoles_Init();
    State_Init();
    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        State_UpdateAndDraw();
        EndDrawing();
    }

    Games_Unload();
    CloseWindow();
    return 0;
}