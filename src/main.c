#include "raylib.h"
#include "states.h"
#include "ui/ui.h"
#include "games.h"
#include "categories.h"
#include "config.h"
#include "play/play.h"
#include "controller.h"
#include "play/playpause.h"
#include "var.h"
#include <stdio.h>

//Length and width of the resolution
#define RESOLUTION_WIDTH 1920
#define RESOLUTION_HEIGHT 1080

//Main function
int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, "Custom Console OS");
    ToggleFullscreen();
    SetTargetFPS(FPS);
    Var_Init();
    Games_Init();
    Categories_Init();
    States_Init();
    Controller_Refresh();
    Var_SetPowerOff(false);
    //Main game loop
    while (!Var_GetPowerOff()) {
        //Pick up controllers being plugged in or unplugged
        Controller_Refresh();
        //Check if buttons are pressed
        if ((!A_DOWN && !IsKeyDown(KEY_ENTER)) && Controller_GetWasPressed_A()) {
            Controller_SetWasPressed_A(false);
        }
        if ((!B_DOWN && !IsKeyDown(KEY_ESCAPE)) && Controller_GetWasPressed_B()) {
            Controller_SetWasPressed_B(false);
        }
        if (!HOME_DOWN && Controller_GetWasPressed_Home()) {
            Controller_SetWasPressed_Home(false);
        }
        //Update and draw on screen
        BeginDrawing();
        States_UpdateAndDraw();
        EndDrawing();
        //Load game images (if not already loaded)
        States_LoadGameImages();
    }
    Play_StopLib(Games_GetDisplayed(CURRENT_GAME));
    Games_UnloadTextures();
    CloseWindow();
    return 0;
}