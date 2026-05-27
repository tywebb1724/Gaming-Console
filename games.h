#ifndef GAMES_H
#define GAMES_H

#include "raylib.h"
#include "ui.h"

//Max amount of games console can hold
#define MAX_GAMES 5

typedef struct {
    char *title;
    char *filePath;
    Texture2D cover;
} Game;

extern Game gameLibrary[];

extern Game gameLeft2;
extern Game gameLeft1;
extern Game gameCenter;
extern Game gameRight1;
extern Game gameRight2;

//Initialize game library
void Games_Init(void);

void Games_LoadTextures(void);
//Unload game textures
void Games_Unload(void);

#endif