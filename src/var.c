#include "var.h"
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "retro_bridge.h"

//Variables for brightness
static float brightness;
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
//Temperature of the CPU in degrees C
static float cpuTemp;
//Clock speed of the CPU in kHz
static int cpuClock;
//Timer for updating diagnostics
static float diagTimer;
//Frame time tracking for the diagnostics overlay
static float frameMsAvg;
static float frameMsPeak;
//Accumulators for the second currently being measured
static float frameMsTotal;
static float frameMsWorst;
static int frameMsCount;
//Emulator fps variables
static float emuFps = 0.0f;
static int emuFrameAccum = 0;


//Initialize the variables
void Var_Init() {
    //Get monitor width and height
    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);
    //Load the fonts into this file
    fontRegular = LoadFontEx("assets/fonts/Exo2-Regular.ttf", 64, NULL, 0);
    fontBold = LoadFontEx("assets/fonts/Exo2-Bold.ttf", 64, NULL, 0);
    //Load the backgrounds
    backgroundBlue = LoadTexture("./assets/covers/logo/BlueBackground.png");
    backgroundRed = LoadTexture("./assets/covers/logo/RedBackground.png");
    backgroundGreen = LoadTexture("./assets/covers/logo/GreenBackground.png");
    backgroundYellow = LoadTexture("./assets/covers/logo/YellowBackground.png");
    //Set display variables as false
    displayBrightness = false;
    displayTheme = false;
    cpuTemp = 0;
    cpuClock = 0;
    diagTimer = DIAG_TIME;
    frameMsAvg = 0;
    frameMsPeak = 0;
    frameMsTotal = 0;
    frameMsWorst = 0;
    frameMsCount = 0;
    emuFps = 0.0f;
    emuFrameAccum = 0;
}

//Get brightness value
float Var_GetBright() {
    return brightness;
}

//Catch bad brightness values
static float Var_ClampBright(float value) {
    //Catch bad brightness values
    if (!(value >= MAX_BRIGHTNESS)) {
        return MAX_BRIGHTNESS;
    }
    if (value > MIN_BRIGHTNESS) {
        return MIN_BRIGHTNESS;
    }
    return value;
}

//Set brightness value
void Var_SetBright(float value) {
    brightness = Var_ClampBright(value);
}

//Set brightness value
void Var_AddBright(float value) {
    brightness = Var_ClampBright(brightness + value);
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
Texture2D Var_NametoBackground(char *c) {
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

//Update CPU temperature
void Var_UpdateTemp() {
    diagTimer += GetFrameTime();
    //Not time to update
    if (diagTimer < DIAG_TIME) {
        return;
    }
    FILE* f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    //If the file will not open, keep the reading file already has
    if (f == NULL) {
        return;
    }
    //Temporary temperature variable (in thousandths of a degree)
    float tempTemp = 0;
    //Scan the file for the temperature and convert to degrees
    if (fscanf(f, "%f", &tempTemp) == 1) {
        cpuTemp = tempTemp / 1000.0f;
    }
    fclose(f);
}

//Get CPU temperature
float Var_GetTemp() {
    return cpuTemp;
}

//Update CPU clock speed
void Var_UpdateClock() {
    //Not time to update
    if (diagTimer < DIAG_TIME) {
        return;
    }
    FILE* f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
    //If the file will not open, keep the reading file already has
    if (f == NULL) {
        return;
    }
    //Scan the file for the temperature and convert to degrees
    fscanf(f, "%d", &cpuClock);
    fclose(f);
}

//Get CPU clock speed
int Var_GetClock() {
    return cpuClock;
}

//Update frame time
void Var_UpdateFrame() {
    float ms = GetFrameTime() * 1000.0f;
    frameMsTotal += ms;
    frameMsCount++;
    //Remember the worst frame in this window
    if (ms > frameMsWorst) {
        frameMsWorst = ms;
    }
    //Not time to publish yet
    if (diagTimer < DIAG_TIME) {
        return;
    }
    if (frameMsCount > 0) {
        frameMsAvg = frameMsTotal / frameMsCount;
    }
    frameMsPeak = frameMsWorst;
    //Start a new window
    frameMsTotal = 0.0f;
    frameMsWorst = 0.0f;
    frameMsCount = 0;
    diagTimer = 0.0f;
}

//Get average frame time
float Var_GetFrameAvg() {
    return frameMsAvg;
}

//Get worst frame time
float Var_GetFrameWorst() {
    return frameMsPeak;
}

//Update emulator FPS
void Var_UpdateEmuFps(void) {
    // ccumulate the core frames produced since last call, every frame
    emuFrameAccum += GetAndResetVRCBCount();

    //Stop if time hasn't passed
    if (diagTimer < DIAG_TIME) {
        return;
    }
    //frames / seconds = FPS
    emuFps = emuFrameAccum / DIAG_TIME;
    emuFrameAccum = 0;
}

//Get emulator FPS
float Var_GetEmuFps(void) {
    return emuFps;
}
