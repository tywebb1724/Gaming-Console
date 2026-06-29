#ifndef STATES_H
#define STATES_H

#include <pthread.h>
#include "raylib.h"
extern int monitorWidth;
extern int monitorHeight;



//Variable to keep track of the amount of games loaded
extern int gamesLoaded;
//Thread for loaded images
extern pthread_t loadThread;
//Spider logo texture
extern Texture2D spiderLogo;

extern Texture2D background_Blue;
extern Texture2D background_Red;
extern Texture2D background_Green;
extern Texture2D background_Yellow;
extern Texture2D currentBackground;
extern double d1;
extern double d2;
extern double d3;


//Initialize the states
void State_Init();
//Update states and variabels and draw the correct screen
void State_UpdateAndDraw();

#endif