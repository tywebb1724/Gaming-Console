#include "categories.h"
#include <stdio.h>
#include "states.h"
#include <pthread.h>

Category categories[CATEGORIES_LEN];

Category categoriesDisplayed[CATEGORIES_ON_SCREEN + 2];

int categoriesIndex;

//Update the categories 
void Categories_Refresh() {
    int offset;
    int targetIndex;
    for (int i = 0; i < CATEGORIES_ON_SCREEN + 2; i++) {
        offset = i - 2;
        targetIndex = (categoriesIndex + offset) % CATEGORIES_LEN;
        if (targetIndex < 0) {
            targetIndex += CATEGORIES_LEN;
        }
        categoriesDisplayed[i] = categories[targetIndex];
    }
}

//Init function for the consoles
void Categories_Init() {

    categories[0].name = "Retro PC & Indie";
    categories[0].id = PC_INDIE;

    categories[1].name = "Retro Nintendo";
    categories[1].id = NINTENDO_RETRO;

    categories[2].name = "Nintendo 3D";
    categories[2].id = NINTENDO_3D;

    categories[3].name = "Handheld Classics";
    categories[3].id = HANDHELD;

    categories[4].name = "Arcade";
    categories[4].id = ARCADE;

    categories[5].name = "Playstation";
    categories[5].id = PLAYSTATION;

    categories[6].name = "Sega";
    categories[6].id = SEGA;

    categoriesIndex = 2;
    Categories_Refresh();
}

//Shift the order of the consoles to the right
void Categories_ScrollRight() {
    categoriesIndex = (categoriesIndex + 1) % CATEGORIES_LEN;
    Categories_Refresh();
    Games_UpdateIndexes(categoriesDisplayed[2].id);
    Games_Refresh();
}

//Shift the order of the consoles to the left
void Categories_ScrollLeft() {
    categoriesIndex = (categoriesIndex - 1 + CATEGORIES_LEN) % CATEGORIES_LEN;
    Categories_Refresh();
    Games_UpdateIndexes(categoriesDisplayed[2].id);
    Games_Refresh();
}
