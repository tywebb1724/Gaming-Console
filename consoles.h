#ifndef CONSOLES_H
#define CONSOLES_H

#include "raylib.h"

#define CONSOLES_ON_SCREEN 5

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

void Consoles_Init(void);

#endif