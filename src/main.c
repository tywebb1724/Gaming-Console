#include "raylib.h"
#include "ui_config.h"
#include "states.h"
#include "ui.h"
#include "games.h"
#include "categories.h"
#include "play.h"
#include "controller_config.h"


//Main function
int main(void)
{
    //Synch refresh rates
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    //Initialize a window 
    InitWindow(1920, 1080, "Custom Console OS");
    //Go fullscreen
    ToggleFullscreen();
    //Initialize state machines
    Games_Init();
    Categories_Init();
    State_Init();
    Play_Init();
    UI_Init();
    Var_Init();
    //Set target FPS to 60
    SetTargetFPS(FPS);
    //Main game loop
    while (!IsKeyPressed(KEY_ESCAPE) && (!HOME_PRESS || !START_PRESS)) {
        //Start drawing
        BeginDrawing();
        //Update console and draw
        State_UpdateAndDraw();
        //Stop drawing
        EndDrawing();
        //Load game imaagesd (if not already loaded)
        State_LoadGameImages();
    }
    //Stop any possibly running game
    Play_Stop(*Games_GetDisplayed(3));
    //Unload game textures and close window
    Games_UnloadTextures();
    //Close window
    CloseWindow();
    return 0;
}