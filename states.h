#ifndef STATES_H
#define STATES_H

#include <pthread.h>
#include "raylib.h"


extern int gamesLoaded;

extern pthread_t loadThread;
extern Texture2D spiderLogo;

typedef enum {
    STATE_BOOT,
    STATE_MAIN_MENU,
    STATE_APP_LAUNCHER,
    STATE_DIAGNOSTICS
} ConsoleState;


//Initialize the states
void State_Init();
//Update states and variabels and draw the correct screen
void State_UpdateAndDraw();

#endif