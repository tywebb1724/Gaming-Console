#include "raylib.h"
#include "states.h"
#include "ui.h"
#include "games.h"
#include "ui_config.h"
#include "categories.h"
#include <stdio.h>
#include <math.h>
#include <sys/personality.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "emulators/libretro/retro_bridge.h"
#include "emulators/libretro/libretro.h"

int monitorWidth;
int monitorHeight;

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void create_blank_brm(const char *filename, int size) {
    uint8_t *data = calloc(size, 1);
    if (!data) return;

    // BRAM format header
    uint8_t fmt[] = {
        0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,
        0x5f,0x5f,0x5f,0x00,0x00,0x00,0x00,0x40,
        0x00,0x7d,0x00,0x7d,0x00,0x7d,0x00,0x7d,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    };

    // Set block count fields
    uint8_t block_count = (size / 64) - 3;
    fmt[0x11] = fmt[0x13] = fmt[0x15] = fmt[0x17] = block_count;

    memcpy(data, fmt, sizeof(fmt));

    FILE *f = fopen(filename, "wb");
    if (f) {
        fwrite(data, 1, size, f);
        fclose(f);
    }

    free(data);
}


//Main function
int main(void)
{
    //Synch refresh rates
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    //Initialize a window 
    InitWindow(1920, 1080, "Custom Console OS");
    
    //Load fonts
    fontRegular = LoadFont("assets/fonts/Exo2-Regular.ttf");
    fontBold = LoadFont("assets/fonts/Exo2-Bold.ttf");

    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);

    ToggleFullscreen();

    //Initialize state machine
    Games_Init();
    Categories_Init();
    State_Init();
    
    //Games_LaunchInit(gameLibrary[40]);
    SetTargetFPS(60);

    //Main game loop
    while (!IsKeyPressed(KEY_ESCAPE) && (!HOME_PRESS || !START_PRESS)) {
    

        
        //Start drawing
        BeginDrawing();
        //Update console and draw
        State_UpdateAndDraw();
        DrawFPS(10,10);
        //Games_LaunchRefresh(gameLibrary[40]);
        //Stop drawing
        EndDrawing();
    }
    Games_StopGame(*gamesDisplayed[3]);
    //Unload game textures and close window
    Games_UnloadTextures();
    //Close window
    CloseWindow();
    return 0;
}