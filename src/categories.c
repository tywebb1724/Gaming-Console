#include "categories.h"

//Arrays to hold all categories and those displayed on the screen
static char *categories[CATEGORIES_LEN];
static char *categoriesDisplayed[CATEGORIES_DISPLAYED];
//Index for the current category
static int categoriesIndex;


//Get a single category from the main array
const char* Categories_Get(int i) {
    return categories[i];
}

//Get a single category from the displayed array
const char* Categories_GetDisplayed(int i) {
    return categoriesDisplayed[i];
}

//Update the displayed categories 
static void Categories_Refresh() {
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
        //Update the displayed array given the index
        categoriesDisplayed[i] = categories[targetIndex];
    }
}

//Init function for the consoles
void Categories_Init() {
    //Set all the categories
    categories[ARCADE_INDEX] = "Arcade";
    categories[HANDHELD_INDEX] = "Handheld Classics";
    categories[NIN3D_INDEX] = "Nintendo 3D";
    categories[NINRET_INDEX] = "Retro Nintendo";
    categories[OTHER_INDEX] = "TurboGrafx/PC/Other";
    categories[SEGA_INDEX] = "Sega";
    categories[PS_INDEX] = "PlayStation";
    //Start with the Nintendo 3D category
    categoriesIndex = NIN3D_INDEX;
    //Update the initial displayed categories
    Categories_Refresh();
}

//Shift the order of the consoles to the right
void Categories_ScrollRight() {
    categoriesIndex = (categoriesIndex + 1) % CATEGORIES_LEN;
    Categories_Refresh();
}

//Shift the order of the consoles to the left
void Categories_ScrollLeft() {
    categoriesIndex = (categoriesIndex - 1 + CATEGORIES_LEN) % CATEGORIES_LEN;
    Categories_Refresh();
}
