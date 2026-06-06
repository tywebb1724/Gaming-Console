#ifndef GAMES_H
#define GAMES_H

#include "raylib.h"
#include "ui.h"

//Max amount of games console can hold
#define MAX_GAMES 60

#define GAMES_ON_SCREEN 5


typedef enum {
    ARCADE,
    HANDHELD,
    NINTENDO_3D,
    NINTENDO_RETRO,
    PC_INDIE,
    SEGA,
    PLAYSTATION
} Categories;

typedef struct {
    char *title;
    char *coverPath;
    Texture2D cover;
    int index;
    char *console;
    float x;
    float y;
    float w;
    float h;
    Categories category;
} Game;

extern Game gameLibrary[MAX_GAMES];

extern Game gamesDisplayed[GAMES_ON_SCREEN + 2];

extern int gamesLen;

//Initialize game library
void Games_Init();
//Load game cover textures
void Games_LoadTextures();
//Update the games displayed for the new category
void Games_UpdateDisplayed(Categories categ);
//Shift the order of the games to the right
void Games_ScrollRight();
//Shift the order of the games to the left
void Games_ScrollLeft();
//Unload game cover textures
void Games_UnloadTextures();

#endif