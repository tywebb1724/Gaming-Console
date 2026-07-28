#include "raylib.h"
#include "states.h"
#include "ui/ui.h"
#include "games.h"
#include "categories.h"
#include "controller_config.h"
#include "config.h"
#include "play/play.h"


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
    Var_Init();
    Games_Init();
    Categories_Init();
    States_Init();
    UI_Init();
    //Set target FPS to 60
    SetTargetFPS(FPS);
    //Main game loop
    while (!IsKeyPressed(KEY_ESCAPE) && (!HOME_DOWN || !START_DOWN)) {
        //Start drawing
        BeginDrawing();
        //Update console and draw
        States_UpdateAndDraw();
        //Stop drawing
        EndDrawing();
        //Load game images (if not already loaded)
        States_LoadGameImages();
    }
    //Stop any possibly running game
    Play_Stop(Games_GetDisplayed(3));
    //Unload game textures and close window
    Games_UnloadTextures();
    //Close window
    CloseWindow();
    return 0;
}