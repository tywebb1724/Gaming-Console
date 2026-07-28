#include "var.h"
#include <stdio.h>
#include <string.h>

//Variables for brightness and brightness circle position
static float brightness;
static float brightCircleX;
//Theme color variables
static Color themeColor1;
static Color themeColor2;
static Color themeColor3;
//Background variables
static Texture2D currentBackground;
static Texture2D backgroundBlue;
static Texture2D backgroundRed;
static Texture2D backgroundGreen;
static Texture2D backgroundYellow;
//Variables to show whether user is scrolling
static ScrollState scrollGames;
static ScrollState scrollCategories;
//Monitor width and height
static float monitorWidth;
static float monitorHeight;
//Types of font
static Font fontRegular;
static Font fontBold;
//Alpha for the select text
static float alphaSelectTxt;
//Variables to indicate whether things are being displayed
static bool displayBrightness;
static bool displayTheme;
static bool displayDiag;


//Initialize the variables
void Var_Init() {
    //Get monitor width and height
    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);
    //Load the fonts into this file
    fontRegular = LoadFont("assets/fonts/Exo2-Regular.ttf");
    fontBold = LoadFont("assets/fonts/Exo2-Bold.ttf");
    //Load the backgrounds
    backgroundBlue = LoadTexture("./assets/covers/logo/BlueBackground.png");
    backgroundRed = LoadTexture("./assets/covers/logo/RedBackground.png");
    backgroundGreen = LoadTexture("./assets/covers/logo/GreenBackground.png");
    backgroundYellow = LoadTexture("./assets/covers/logo/YellowBackground.png");
    //Set display variables as false
    displayBrightness = false;
    displayTheme = false;
}


//Get brightness value
float Var_GetBright() {
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

//Get alpha select text
float Var_GetAlphaSelect() {
    return alphaSelectTxt;
}

//Set alpha select text
void Var_SetAlphaSelect(float value) {
    alphaSelectTxt = value;
}

//Set alpha select text
void Var_AddAlphaSelect(float value) {
    alphaSelectTxt += value;
}

//Get the current background
Texture2D Var_GetBackground() {
    return currentBackground;
}

//Set the current background
void Var_SetBackground(Texture2D background) {
    currentBackground = background;
}


//Extract a background from a color name
const Texture2D Var_NametoBackground(char *c) {
    //Return the corresponding background for each color name
    if (strcmp(c, "BLUE") == 0) {
        return backgroundBlue;
    }
    else if (strcmp(c, "RED") == 0) {
        return backgroundRed;
    }
    else if (strcmp(c, "GREEN") == 0) {
        return backgroundGreen;
    }
    else if (strcmp(c, "YELLOW") == 0) {
        return backgroundYellow;
    }
    //Fallback case
    return backgroundBlue;
}

//Extract a name from the color
static const char *Var_ColorToName(Color c) {
    //Return the corresponding name depending on rgba values
    if (c.r == BLUE.r && c.g == BLUE.g && c.b == BLUE.b && c.a == BLUE.a)
        return "BLUE";
    if (c.r == RED.r && c.g == RED.g && c.b == RED.b && c.a == RED.a)
        return "RED";
    if (c.r == BLACK.r && c.g == BLACK.g && c.b == BLACK.b && c.a == BLACK.a)
        return "BLACK";
    if (c.r == GREEN.r && c.g == GREEN.g && c.b == GREEN.b && c.a == GREEN.a)
        return "GREEN";
    if (c.r == YELLOW.r && c.g == YELLOW.g && c.b == YELLOW.b && c.a == YELLOW.a)
        return "YELLOW";
    if (c.r == WHITE.r && c.g == WHITE.g && c.b == WHITE.b && c.a == WHITE.a)
        return "WHITE";
    //Fallback case
    return "UNKNOWN";
}

//Update the UI text file
void Var_UpdateUIFile() {
    //Open the file
    FILE *f = fopen("/home/tywebb1724/Desktop/Gaming-Console/assets/system/ui.txt", "w");
    //If it opens correctly, write the variables
    if (f) {
        fprintf(f, "%s\n", Var_ColorToName(Var_GetColor1()));
        fprintf(f, "%s\n", Var_ColorToName(Var_GetColor2()));
        fprintf(f, "%s\n", Var_ColorToName(Var_GetColor3()));
        fprintf(f, "%f\n", Var_GetBright());
        fprintf(f, "%f\n", Var_GetBrightX());
        fprintf(f, "%d\n", Var_GetDiag());
        fclose(f);
    }
}

//Get bold font
Font Var_GetFontBold() {
    return fontBold;
}

//Get regular font
Font Var_GetFontRegular() {
    return fontRegular;
}

//Get monitor width
float Var_GetMonitorWidth() {
    return monitorWidth;
}

//Get monitor height
float Var_GetMonitorHeight() {
    return monitorHeight;
}

//Get whether brightness options are being displayed
bool Var_GetDisplayBright() {
    return displayBrightness;
}

//Set whether brightness options are being displayed
void Var_SetDisplayBright(bool value) {
    displayBrightness = value;
}

//Get whether theme options are being displayed
bool Var_GetDisplayTheme() {
    return displayTheme;
}

//Set whether theme options are being displayed
void Var_SetDisplayTheme(bool value) {
    displayTheme = value;
}
