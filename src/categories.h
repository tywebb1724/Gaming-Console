#ifndef CATEGORIES_H
#define CATEGORIES_H

//Macros for the categories
#define CATEGORIES_ON_SCREEN 3
#define CATEGORIES_LEN 7

//Get a single category from the main array
const char* Categories_Get(int i);
//Get a single category from the displayed array
const char* Categories_GetDisplayed(int i);
//Init function for the categories
void Categories_Init();
//Shift the order of the categories to the right
void Categories_ScrollRight();
//Shift the order of the categories to the left
void Categories_ScrollLeft();


#endif