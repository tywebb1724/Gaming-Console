#ifndef CATEGORIES_H
#define CATEGORIES_H

#include "raylib.h"

#define CATEGORIES_ON_SCREEN 3
#define CATEGORIES_LEN 7

typedef struct {
    char *name;
    int index;
} Category;


extern Category categories[CATEGORIES_LEN];

extern Category categoriesDisplayed[CATEGORIES_ON_SCREEN + 2];

//Init function for the categories
void Categories_Init();

//Shift the order of the categories to the right
void Categories_ScrollRight();

//Shift the order of the categories to the left
void Categories_ScrollLeft();


#endif