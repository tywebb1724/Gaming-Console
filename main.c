#include "raylib.h"
#include "states.h"
#include "ui.h"
#include "games.h"


int main(void)
{
    // Initialize a window 800 pixels wide by 450 pixels high
    InitWindow(800, 450, "Custom Console OS");
    SetTargetFPS(60);
    
    //Initialize state machine
    Games_Init();
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