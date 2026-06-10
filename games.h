#ifndef GAMES_H
#define GAMES_H

#include "raylib.h"
#include "ui.h"

//Max amount of games console can hold
#define MAX_GAMES 60
#define GAMES_LEN 49

#define GAMES_ON_SCREEN 5

extern int start_LeftIndex;
extern int end_LeftIndex;
extern int start_RightIndex;
extern int end_RightIndex;

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
    char *console;
    float x;
    float y;
    float w;
    float h;
    Categories category;
} Game;

extern Game gameLibrary[MAX_GAMES];

extern Game* gamesDisplayed[GAMES_ON_SCREEN + 2];
extern Game* newGamesDisplayed[GAMES_ON_SCREEN];

extern bool texturesLoaded;

extern Image LoadedImages[GAMES_LEN];
extern bool isLoaded[GAMES_LEN];
extern bool isTextureUploaded[GAMES_LEN];


void Game_New_Indexes();

void Games_UpdateIndexes(Categories categ);
void Games_NewRefresh();

//Update the indexes of the current game category
void Games_UpdateIndexes(Categories categ);
//Update the games displayed for the new category
void Games_Refresh();
//Initialize game library
void Games_Init();
//Load game cover textures
void *Games_LoadImages();
//Shift the order of the games to the right
void Games_ScrollRight();
//Shift the order of the games to the left
void Games_ScrollLeft();
//Unload game cover textures
void Games_UnloadTextures();

#endif