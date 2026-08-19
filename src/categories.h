#ifndef CATEGORIES_H
#define CATEGORIES_H

//Macros for the categories
#define CATEGORIES_ON_SCREEN 3
#define CATEGORIES_LEN 7
#define CATEGORIES_DISPLAYED 5
//Indexes of categories
#define ARCADE_INDEX 0
#define HANDHELD_INDEX 1
#define NIN3D_INDEX 2
#define NINRET_INDEX 3
#define OTHER_INDEX 4
#define SEGA_INDEX 5
#define PS_INDEX 6

//Get a single category from the main array
const char* Categories_Get(int i);
//Get a single category from the displayed array
const char* Categories_GetDisplayed(int i);
//Init function for the categories
void Categories_Init(void);
//Shift the order of the categories to the right
void Categories_ScrollRight(void);
//Shift the order of the categories to the left
void Categories_ScrollLeft(void);


#endif