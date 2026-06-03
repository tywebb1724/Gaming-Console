#ifndef CONSOLES_H
#define CONSOLES_H

#include "raylib.h"

#define CONSOLES_ON_SCREEN 3
#define CONSOLES_LEN 5

typedef struct {
    char *name;
    Texture2D cover;
    int index;
    float x;
    float y;
    float w;
    float h;
} Console;

#define MAX_CONSOLES 5

extern Console consoleLibrary[MAX_CONSOLES];

extern Console consolesDisplayed[CONSOLES_LEN];

//Init function for the consoles
void Consoles_Init();

//Load console cover textures
void Consoles_LoadTextures();

//Shift the order of the consoles to the right
void Consoles_ScrollRight();

//Unload console textures
void Consoles_ScrollLeft();

//Unload console cover textures
void Consoles_UnloadTextures();

#endif