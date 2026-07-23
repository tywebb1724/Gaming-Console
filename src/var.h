#ifndef VAR_H
#define VAR_H

#include "raylib.h"

//Scrolling states
typedef enum {
    SCROLL_NO,
    SCROLL_LEFT,
    SCROLL_RIGHT
} ScrollState;


//Get brightness value
float Vars_GetBright();
//Set brightness value
void Var_SetBright(float value);
//Set brightness value
void Var_AddBright(float value);
//Get brightness value
float Var_GetBrightX();
//Set brightness value
void Var_SetBrightX(float value);
//Set brightness value
void Var_AddBrightX(float value);
//Get theme color 1
Color Var_GetColor1();
//Get theme color 2
Color Var_GetColor2();
//Get theme color 3
Color Var_GetColor3();
//Set theme color 1
void Var_SetColor1(Color color);
//Set theme color 2
void Var_SetColor2(Color color);
//Set theme color 3
void Var_SetColor3(Color color);
//Get scroll games
ScrollState Var_GetScrollGames();
//Get scroll categories
ScrollState Var_GetScrollCateg();
//Set scroll games
void Var_SetScrollGames(ScrollState state);
//Set scroll categories
void Var_SetScrollCateg(ScrollState state);

#endif