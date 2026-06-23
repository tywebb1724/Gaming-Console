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
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    //Initialize a window 
    InitWindow(1920, 1080, "Custom Console OS");
    
    //Load fonts
    //fontRegular = LoadFont("assets/fonts/Exo2-Regular.ttf");
    //fontBold = LoadFont("assets/fonts/Exo2-Bold.ttf");

    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);

    ToggleFullscreen();

    
    //Initialize state machine
    Games_Init();
    Categories_Init();
    State_Init();
    Games_LaunchInit(gameLibrary[11]);

    SetTargetFPS(0);

    //Main game loop
    while (!WindowShouldClose()) {
        
        //Start drawing
        BeginDrawing();
        //Update console and draw
        //State_UpdateAndDraw();
        Games_LaunchRefresh(gameLibrary[11]);
        //Stop drawing
        EndDrawing();
    }
    //Unload game textures and close window
    Games_UnloadTextures();
    //Close window
    CloseWindow();
    return 0;
}