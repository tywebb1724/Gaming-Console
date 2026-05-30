#ifndef GAMES_H
#define GAMES_H

#include "raylib.h"
#include "ui.h"

//Max amount of games console can hold
#define MAX_GAMES 10

#define GAMES_ON_SCREEN 5

typedef struct {
    char *title;
    char *filePath;
    Texture2D cover;
    int index;
    char *console;
    float x;
    float y;
    float w;
    float h;
} Game;

extern Game gameLibrary[];

extern Game gamesDisplayed[GAMES_ON_SCREEN + 2];

extern int gamesLen;

//Initialize game library
void Games_Init(void);

void Games_LoadTextures(void);
void Games_ScrollRight(void);
void Games_ScrollLeft(void);
//Unload game textures
void Games_Unload(void);

#endif