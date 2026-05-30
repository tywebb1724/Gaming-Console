#include "consoles.h"

Console consoleLibrary[CONSOLES_ON_SCREEN];

Console consolesDisplayed[CONSOLES_ON_SCREEN];

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

    for (int i = 0; i++; i < CONSOLES_ON_SCREEN) {
        consolesDisplayed[i].index = i;
    }

    for (int i = 0; i++; i < CONSOLES_ON_SCREEN) {
        consolesDisplayed[i] = consoleLibrary[consolesDisplayed[i].index];
    }

}

void Consoles_Load_Textures() {
    consoleLibrary[0].cover = LoadTexture("./assets/Nintendo.png");
    consoleLibrary[1].cover = LoadTexture("./assets/Sega.png");
    consoleLibrary[2].cover = LoadTexture("./assets/Playstation.png");
    consoleLibrary[3].cover = LoadTexture("./assets/Atari.png");
    consoleLibrary[4].cover = LoadTexture("./assets/OtherConsole.png");
}

void Console_ScrollRight(void) {
    for (int i = 0; i < CONSOLES_ON_SCREEN; i++) {
        consolesDisplayed[i].index -= 1;
        if (consolesDisplayed[i].index == -1) {
            consolesDisplayed[i].index = CONSOLES_ON_SCREEN - 1;
        }
    }
    for (int i = 0; i < CONSOLES_ON_SCREEN + 2; i++) {
        consolesDisplayed[i] = consoleLibrary[consolesDisplayed[i].index];
    }
}

void Consoles_ScrollLeft(void) {
    for (int i = 0; i < CONSOLES_ON_SCREEN + 2; i++) {
        consolesDisplayed[i].index += 1;
        if (consolesDisplayed[i].index == CONSOLES_ON_SCREEN) {
            consolesDisplayed[i].index = 0;
        }
    }
    for (int i = 0; i < CONSOLES_ON_SCREEN + 2; i++) {
        consolesDisplayed[i] = consoleLibrary[consolesDisplayed[i].index];
    }
}
    
//Unload game textures
void Consoles_Unload(void) {
    for (int i = 0; i < CONSOLES_ON_SCREEN; i++) {
        UnloadTexture(consoleLibrary[i].cover);
    }
}