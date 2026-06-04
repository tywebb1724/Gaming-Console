#include "categories.h"
#include <stdio.h>

Category categories[CATEGORIES_LEN];

Category categoriesDisplayed[CATEGORIES_ON_SCREEN + 2];

//Init function for the consoles
void Categories_Init() {

    categories[0].name = "Sega Central";
    categories[0].index = 0;

    categories[1].name = "Retro Nintendo";
    categories[1].index = 1;

    categories[2].name = "Nintendo 3D";
    categories[2].index = 2;

    categories[3].name = "Handheld Classics";
    categories[3].index = 3;

    categories[4].name = "Sony Playstation";
    categories[4].index = 4;

    categories[5].name = "Retro PC & Indie";
    categories[5].index = 5;

    categories[6].name = "Arcade";
    categories[6].index = 5;

    for (int i = 0; i < CATEGORIES_ON_SCREEN + 2; i++) {
        categoriesDisplayed[i].index = i;
    }

    for (int i = 0; i < CATEGORIES_ON_SCREEN + 2; i++) {
        categoriesDisplayed[i] = categories[categoriesDisplayed[i].index];
    }

}

//Shift the order of the consoles to the right
void Categories_ScrollRight() {
    for (int i = 0; i < CATEGORIES_ON_SCREEN + 2; i++) {
        categoriesDisplayed[i].index -= 1;
        if (categoriesDisplayed[i].index == -1) {
            categoriesDisplayed[i].index = CATEGORIES_LEN - 1;
        }
    }
    for (int i = 0; i < CATEGORIES_ON_SCREEN + 2; i++) {
        categoriesDisplayed[i] = categories[categoriesDisplayed[i].index];
    }
}

//Shift the order of the consoles to the left
void Categories_ScrollLeft() {
    for (int i = 0; i < CATEGORIES_ON_SCREEN + 2; i++) {
        categoriesDisplayed[i].index += 1;
        if (categoriesDisplayed[i].index == CATEGORIES_LEN) {
            categoriesDisplayed[i].index = 0;
        }
    }
    for (int i = 0; i < CATEGORIES_ON_SCREEN + 2; i++) {
        categoriesDisplayed[i] = categories[categoriesDisplayed[i].index];
    }
}
