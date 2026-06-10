#ifndef CATEGORIES_H
#define CATEGORIES_H

#include <stdio.h>
#include <pthread.h>
#include "raylib.h"
#include "games.h"
#include "states.h"

//Macros for the categories
#define CATEGORIES_ON_SCREEN 3
#define CATEGORIES_LEN 7

//Struct for the categories
typedef struct {
    char *name;
    Categories id;
} Category;

//Arrays to hold all categories and those displayed on the screen
extern Category categories[CATEGORIES_LEN];
extern Category categoriesDisplayed[CATEGORIES_ON_SCREEN + 2];


//Init function for the categories
void Categories_Init();
//Shift the order of the categories to the right
void Categories_ScrollRight();
//Shift the order of the categories to the left
void Categories_ScrollLeft();


#endif