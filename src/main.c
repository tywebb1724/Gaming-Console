#include "raylib.h"
#include "states.h"
#include "ui/ui.h"
#include "games.h"
#include "categories.h"
#include "controller_config.h"
#include "config.h"
#include "play/play.h"
#include "controller.h"
#include "play/playpause.h"


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
    //Set target FPS to 60
    SetTargetFPS(FPS);
    //Find any controllers that are already plugged in
    Controller_Refresh();
    //Main game loop
    while (!IsKeyPressed(KEY_ESCAPE) && (!HOME_DOWN || !START_DOWN)) {
        //Pick up controllers being plugged in or unplugged
        Controller_Refresh();
        //Check if a button is pressed
        if (!A_DOWN && Controller_GetWasPressed_A()) {
            Controller_SetWasPressed_A(false);
        }
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