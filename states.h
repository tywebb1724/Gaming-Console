#ifndef STATES_H
#define STATES_H

#include <pthread.h>
#include "raylib.h"

//States of the console
typedef enum {
    STATE_BOOT,
    STATE_MAIN_MENU,
    STATE_APP_LAUNCHER,
    STATE_DIAGNOSTICS
} ConsoleState;

//Variable to keep track of the amount of games loaded
extern int gamesLoaded;
//Thread for loaded images
extern pthread_t loadThread;
//Spider logo texture
extern Texture2D spiderLogo;


//Initialize the states
void State_Init();
//Update states and variabels and draw the correct screen
void State_UpdateAndDraw();

#endif