#include "games.h"
#include "raylib.h"
#include "ui.h"

Game gameLibrary[MAX_GAMES];

Game gamesDisplayed[GAMES_ON_SCREEN + 2];

int gamesLen = 8;

static bool texturesLoaded = false;

//Initialize game library
void Games_Init() {
    //Set up game 1
    gameLibrary[0].title = "Super Mario Brothers";
    gameLibrary[0].filePath = "path";
    gameLibrary[0].cover.id = 0;
    gameLibrary[0].index = 0;
    //Set up game 2
    gameLibrary[1].title = "Link To The Past";
    gameLibrary[1].filePath = "path";
    gameLibrary[1].cover.id = 0;
    gameLibrary[1].index = 1;
    //Set up game 3
    gameLibrary[2].title = "Sonic The Hedgehog";
    gameLibrary[2].filePath = "path";
    gameLibrary[2].cover.id = 0;
    gameLibrary[2].index = 2;

    gameLibrary[3].title = "Street Fighter";
    gameLibrary[3].filePath = "path";
    gameLibrary[3].cover.id = 0;
    gameLibrary[3].index = 3;

    gameLibrary[4].title = "Mega Man 4";
    gameLibrary[4].filePath = "path";
    gameLibrary[4].cover.id = 0;
    gameLibrary[4].index = 4;

    gameLibrary[5].title = "Super Mario 64";
    gameLibrary[5].filePath = "path";
    gameLibrary[5].cover.id = 0;
    gameLibrary[5].index = 5;

    gameLibrary[6].title = "PAC-MAN";
    gameLibrary[6].filePath = "path";
    gameLibrary[6].cover.id = 0;
    gameLibrary[6].index = 6;

    gameLibrary[7].title = "Donkey Kong 3";
    gameLibrary[7].filePath = "path";
    gameLibrary[7].cover.id = 0;
    gameLibrary[7].index = 7;

    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i].index = i;
    }

    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i] = gameLibrary[gamesDisplayed[i].index];
    }

    texturesLoaded = false;
}

//Load game cover textures
void Games_LoadTextures() {
    if (texturesLoaded) {
        return;
    }
    gameLibrary[0].cover = LoadTexture("./assets/SuperMarioBrothers.png");
    gameLibrary[1].cover = LoadTexture("./assets/LinkToThePast.png");
    gameLibrary[2].cover = LoadTexture("./assets/SonicTheHedgehog.png");
    gameLibrary[3].cover = LoadTexture("./assets/StreetFighter.png");
    gameLibrary[4].cover = LoadTexture("./assets/MegaMan4.png");
    gameLibrary[5].cover = LoadTexture("./assets/SuperMario64.png");
    gameLibrary[6].cover = LoadTexture("./assets/Pac-Man.png");
    gameLibrary[7].cover = LoadTexture("./assets/DonkeyKong3.png");

    texturesLoaded = true;
}

//Shift the order of the games to the right
void Games_ScrollRight() {
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i].index -= 1;
        if (gamesDisplayed[i].index == -1) {
            gamesDisplayed[i].index = gamesLen - 1;
        }
    }
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i] = gameLibrary[gamesDisplayed[i].index];
    }
}

//Shift the order of the gamees to the left
void Games_ScrollLeft() {
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i].index += 1;
        if (gamesDisplayed[i].index == gamesLen) {
            gamesDisplayed[i].index = 0;
        }
    }
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i] = gameLibrary[gamesDisplayed[i].index];
    }
}
    
//Unload game cover textures
void Games_Unload() {
    for (int i = 0; i < gamesLen; i++) {
        UnloadTexture(gameLibrary[i].cover);
    }

    texturesLoaded = false;
}