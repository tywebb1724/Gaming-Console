#include "games.h"
#include "raylib.h"
#include "ui.h"

Game gameLibrary[MAX_GAMES];

Game gameLeft2;
Game gameLeft1;
Game gameCenter;
Game gameRight1;
Game gameRight2;

static bool texturesLoaded = false;

//Initialize game library
void Games_Init(void) {
    //Set up game 1
    gameLibrary[0].title = "Super Mario Brothers";
    gameLibrary[0].filePath = "path";
    gameLibrary[0].cover.id = 0;
    //Set up game 2
    gameLibrary[1].title = "Link To The Past";
    gameLibrary[1].filePath = "path";
    gameLibrary[1].cover.id = 0;
    //Set up game 3
    gameLibrary[2].title = "Sonic The Hedgehog";
    gameLibrary[2].filePath = "path";
    gameLibrary[2].cover.id = 0;

    gameLibrary[3].title = "Street Fighter";
    gameLibrary[3].filePath = "path";
    gameLibrary[3].cover.id = 0;

    gameLibrary[4].title = "Mega Man 4";
    gameLibrary[4].filePath = "path";
    gameLibrary[4].cover.id = 0;

    gameLeft2 = gameLibrary[0];
    gameLeft1 = gameLibrary[1];
    gameCenter = gameLibrary[2];
    gameRight1 = gameLibrary[3];
    gameRight2 = gameLibrary[4];

    texturesLoaded = false;
}

void Games_LoadTextures(void) {
    if (texturesLoaded) {
        return;
    }
    gameLibrary[0].cover = LoadTexture("./assets/SuperMarioBrothers.png");
    gameLibrary[1].cover = LoadTexture("./assets/LinkToThePast.png");
    gameLibrary[2].cover = LoadTexture("./assets/SonicTheHedgehog.png");
    gameLibrary[3].cover = LoadTexture("./assets/StreetFighter.png");
    gameLibrary[4].cover = LoadTexture("./assets/MegaMan4.png");

    texturesLoaded = true;
}

//Unload game textures
void Games_Unload(void) {
    for (int i = 0; i < MAX_GAMES; i++) {
        UnloadTexture(gameLibrary[i].cover);
    }

    texturesLoaded = false;
}