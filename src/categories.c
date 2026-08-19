#include "categories.h"
#include <stddef.h>

//Arrays to hold all categories and those displayed on the screen
static const char *categories[CATEGORIES_LEN];
static const char *categoriesDisplayed[CATEGORIES_DISPLAYED];
//Index for the current category
static int categoriesIndex;


//Get a single category from the main array
const char* Categories_Get(int i) {
    //Check the index
    if (i < 0 || i >= CATEGORIES_LEN) {
        return NULL;
    }
    return categories[i];
}

//Get a single category from the displayed array
const char* Categories_GetDisplayed(int i) {
    //Check the index
    if (i < 0 || i >= CATEGORIES_DISPLAYED) {
        return NULL;
    }
    return categoriesDisplayed[i];
}

//Update the displayed categories 
static void Categories_Refresh(void) {
    int offset;
    int targetIndex;
    //Cycle through the categories
    for (int i = 0; i < CATEGORIES_DISPLAYED; i++) {
        offset = i - 2;
        targetIndex = (categoriesIndex + offset) % CATEGORIES_LEN;
        //If the offset causes the index to be negative, add the category length
        if (targetIndex < 0) {
            targetIndex += CATEGORIES_LEN;
        }
        categoriesDisplayed[i] = categories[targetIndex];
    }
}

//Init function for the consoles
void Categories_Init(void) {
    categories[ARCADE_INDEX] = "Arcade";
    categories[HANDHELD_INDEX] = "Handheld Classics";
    categories[NIN3D_INDEX] = "Nintendo 3D";
    categories[NINRET_INDEX] = "Retro Nintendo";
    categories[OTHER_INDEX] = "TurboGrafx/PC/Other";
    categories[SEGA_INDEX] = "Sega";
    categories[PS_INDEX] = "PlayStation";
    categoriesIndex = NIN3D_INDEX;
    Categories_Refresh();
}

//Shift the order of the consoles to the right
void Categories_ScrollRight(void) {
    categoriesIndex = (categoriesIndex + 1) % CATEGORIES_LEN;
    Categories_Refresh();
}

//Shift the order of the consoles to the left
void Categories_ScrollLeft(void) {
    categoriesIndex = (categoriesIndex - 1 + CATEGORIES_LEN) % CATEGORIES_LEN;
    Categories_Refresh();
}
