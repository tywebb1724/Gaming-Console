#include "consoles.h"
#include <stdio.h>

Console consoleLibrary[CONSOLES_LEN];

Console consolesDisplayed[CONSOLES_LEN];

//Init function for the consoles
void Consoles_Init() {

    consoleLibrary[0].name = "Nintendo";
    consoleLibrary[0].cover.id = 0;
    consoleLibrary[0].index = 0;

    consoleLibrary[1].name = "Sega";
    consoleLibrary[1].cover.id = 0;
    consoleLibrary[1].index = 1;

    consoleLibrary[2].name = "PlayStation";
    consoleLibrary[2].cover.id = 0;
    consoleLibrary[2].index = 2;

    consoleLibrary[3].name = "Atari";
    consoleLibrary[3].cover.id = 0;
    consoleLibrary[3].index = 3;

    consoleLibrary[4].name = "Other";
    consoleLibrary[4].cover.id = 0;
    consoleLibrary[4].index = 4;

    for (int i = 0; i < CONSOLES_LEN; i++) {
        consolesDisplayed[i].index = i;
    }

    for (int i = 0; i < CONSOLES_LEN; i++) {
        consolesDisplayed[i] = consoleLibrary[consolesDisplayed[i].index];
    }

}

//Load console cover textures
void Consoles_LoadTextures() {
    consoleLibrary[0].cover = LoadTexture("./assets/Nintendo.png");
    consoleLibrary[1].cover = LoadTexture("./assets/Sega.png");
    consoleLibrary[2].cover = LoadTexture("./assets/Playstation.png");
    consoleLibrary[3].cover = LoadTexture("./assets/Atari.png");
    consoleLibrary[4].cover = LoadTexture("./assets/OtherConsole.png");
}

//Shift the order of the consoles to the right
void Consoles_ScrollRight() {
    for (int i = 0; i < CONSOLES_LEN; i++) {
        consolesDisplayed[i].index -= 1;
        if (consolesDisplayed[i].index == -1) {
            consolesDisplayed[i].index = CONSOLES_LEN - 1;
        }
    }
    for (int i = 0; i < CONSOLES_LEN; i++) {
        consolesDisplayed[i] = consoleLibrary[consolesDisplayed[i].index];
    }
}

//Shift the order of the consoles to the left
void Consoles_ScrollLeft() {
    for (int i = 0; i < CONSOLES_LEN; i++) {
        consolesDisplayed[i].index += 1;
        if (consolesDisplayed[i].index == CONSOLES_LEN) {
            consolesDisplayed[i].index = 0;
        }
    }
    for (int i = 0; i < CONSOLES_LEN; i++) {
        consolesDisplayed[i] = consoleLibrary[consolesDisplayed[i].index];
    }
}
    
//Unload console cover textures
void Consoles_Unload() {
    for (int i = 0; i < CONSOLES_ON_SCREEN; i++) {
        UnloadTexture(consoleLibrary[i].cover);
    }
}