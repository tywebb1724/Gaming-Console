#include "var.h"

static float brightness;
static float brightCircleX;

static Color themeColor1;
static Color themeColor2;
static Color themeColor3;

// Variables to show whether user is scrolling
ScrollState scrollGames;
ScrollState scrollCategories;

bool displayDiag;

//Get brightness value
float Vars_GetBright() {
    return brightness;
}

//Set brightness value
void Var_SetBright(float value) {
    brightness = value;
}

//Set brightness value
void Var_AddBright(float value) {
    brightness += value;
}

//Get brightness value
float Var_GetBrightX() {
    return brightCircleX;
}

//Set brightness value
void Var_SetBrightX(float value) {
    brightCircleX = value;
}

//Set brightness value
void Var_AddBrightX(float value) {
    brightCircleX += value;
}

//Get theme color 1
Color Var_GetColor1() {
    return themeColor1;
}

//Get theme color 2
Color Var_GetColor2() {
    return themeColor2;
}

//Get theme color 3
Color Var_GetColor3() {
    return themeColor3;
}

//Set theme color 1
void Var_SetColor1(Color color) {
    themeColor1 = color;
}

//Set theme color 2
void Var_SetColor2(Color color) {
    themeColor2 = color;
}

//Set theme color 3
void Var_SetColor3(Color color) {
    themeColor3 = color;
}

//Get scroll games
ScrollState Var_GetScrollGames() {
    return scrollGames;
}

//Get scroll categories
ScrollState Var_GetScrollCateg() {
    return scrollCategories;
}

//Set scroll games
void Var_SetScrollGames(ScrollState state) {
    scrollGames = state;
}

//Set scroll categories
void Var_SetScrollCateg(ScrollState state) {
    scrollCategories = state;
}

//Get display diagnostics
bool Var_GetDiag() {
    return displayDiag;
}

//Set display diagnostics
void Var_SetDiag(bool value) {
    displayDiag = value;
}