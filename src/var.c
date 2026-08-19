#include "var.h"
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "retro_bridge.h"
#include <dirent.h>

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
//Power off variable
static bool powerOff;

//Path for the cpu temperature
static char cpuThermalPath[THERM_PATH_LEN] = "";
//Emulator fps variables
static float emuFps = 0.0f;
static int emuFrameAccum = 0;


//Initialize the variables
void Var_Init(void) {
    monitorWidth = GetMonitorWidth(0);
    monitorHeight = GetMonitorHeight(0);
    fontRegular = LoadFontEx("assets/fonts/Exo2-Regular.ttf", 64, NULL, 0);
    fontBold = LoadFontEx("assets/fonts/Exo2-Bold.ttf", 64, NULL, 0);
    backgroundBlue = LoadTexture("assets/images/other/blue_background.png");
    backgroundRed = LoadTexture("assets/images/other/red_background.png");
    backgroundGreen = LoadTexture("assets/images/other/green_background.png");
    backgroundYellow = LoadTexture("assets/images/other/yellow_background.png");
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
float Var_GetBright(void) {
    return brightness;
}

//Catch bad brightness values
static float Var_ClampBright(float value) {
    //If too low, return max brightness
    if (!(value >= MAX_BRIGHTNESS)) {
        return MAX_BRIGHTNESS;
    }
    //If too high, return min brightess
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
Color Var_GetColor1(void) {
    return themeColor1;
}

//Get theme color 2
Color Var_GetColor2(void) {
    return themeColor2;
}

//Get theme color 3
Color Var_GetColor3(void) {
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
ScrollState Var_GetScrollGames(void) {
    return scrollGames;
}

//Get scroll categories
ScrollState Var_GetScrollCateg(void) {
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
bool Var_GetDiag(void) {
    return displayDiag;
}

//Set display diagnostics
void Var_SetDiag(bool value) {
    displayDiag = value;
}

//Get alpha select text
float Var_GetAlphaSelect(void) {
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
Texture2D Var_GetBackground(void) {
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
void Var_UpdateUIFile(void) {
    FILE *f = fopen("assets/txt/ui.txt", "w");
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
Font Var_GetFontBold(void) {
    return fontBold;
}

//Get regular font
Font Var_GetFontRegular(void) {
    return fontRegular;
}

//Get monitor width
float Var_GetMonitorWidth(void) {
    return monitorWidth;
}

//Get monitor height
float Var_GetMonitorHeight(void) {
    return monitorHeight;
}

//Get whether brightness options are being displayed
bool Var_GetDisplayBright(void) {
    return displayBrightness;
}

//Set whether brightness options are being displayed
void Var_SetDisplayBright(bool value) {
    displayBrightness = value;
}

//Get whether theme options are being displayed
bool Var_GetDisplayTheme(void) {
    return displayTheme;
}

//Set whether theme options are being displayed
void Var_SetDisplayTheme(bool value) {
    displayTheme = value;
}

//Find the path to the cpu temperature
void Var_FindThermalZone(void) {
    DIR* d = opendir("/sys/class/thermal");
    //If doesn't open successfully, exit
    if (!d) return;
    struct dirent* entry;
    //Read all the entries in the directory
    while ((entry = readdir(d)) != NULL) {
        //If not the correct entry, go to next one
        if (strncmp(entry->d_name, "thermal_zone", 12) != 0) continue;
        char typePath[300];
        snprintf(typePath, sizeof(typePath), "/sys/class/thermal/%s/type", entry->d_name);
        FILE* tf = fopen(typePath, "r");
        //If doesn't open successfully, exit
        if (!tf) continue;
        char type[128] = "";
        //Look in file for the right information
        if (fgets(type, sizeof(type), tf)) {
            //Common CPU-zone naming across vendors: "cpu", "soc", "cpu-thermal", "x86_pkg_temp"
            if (strstr(type, "cpu") || strstr(type, "soc") || strstr(type, "pkg")) {
                snprintf(cpuThermalPath, sizeof(cpuThermalPath),
                         "/sys/class/thermal/%s/temp", entry->d_name);
                fclose(tf);
                closedir(d);
                return;
            }
        }
        fclose(tf);
    }
    closedir(d);
}

//Update CPU temperature
void Var_UpdateTemp(void) {
    diagTimer += GetFrameTime();
    //If less than the update time, exit
    if (diagTimer < DIAG_TIME) return;
    //If no path, exit
    if (cpuThermalPath[0] == '\0') return;
    FILE* f = fopen(cpuThermalPath, "r");
    //If doesn't open successfully, exit
    if (f == NULL) return;
    float tempTemp = 0;
    //Get the temperature
    if (fscanf(f, "%f", &tempTemp) == 1) {
        cpuTemp = tempTemp / DEGREE_CONVERSION;
    }
    fclose(f);
}

//Get CPU temperature
float Var_GetTemp(void) {
    return cpuTemp;
}

//Update CPU clock speed
void Var_UpdateClock(void) {
    int highest = 0;
    //Cycle through possible cores
    for (int i = 0; i < MAX_CPU_CORES; i++) { 
        char path[128];
        snprintf(path, sizeof(path),
                 "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", i);
        FILE* f = fopen(path, "r");
        //If file doesn't open correctly, move to the next one
        if (!f) break;
        //Get the frequency
        int freq = 0;
        //If frequency is higher than current highest, set it as current highest
        if (fscanf(f, "%d", &freq) == 1 && freq > highest) {
            highest = freq;
        }
        fclose(f);
    }
    cpuClock = highest;
}

//Get CPU clock speed
int Var_GetClock(void) {
    return cpuClock;
}

//Update frame time
void Var_UpdateFrame(void) {
    float ms = GetFrameTime() * MICRO_TO_MILLI;
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
    //If time has passed, calculate average time
    if (frameMsCount > 0) {
        frameMsAvg = frameMsTotal / frameMsCount;
    }
    frameMsPeak = frameMsWorst;
    frameMsTotal = 0.0f;
    frameMsWorst = 0.0f;
    frameMsCount = 0;
    diagTimer = 0.0f;
}

//Get average frame time
float Var_GetFrameAvg(void) {
    return frameMsAvg;
}

//Get worst frame time
float Var_GetFrameWorst(void) {
    return frameMsPeak;
}

//Update emulator FPS
void Var_UpdateEmuFps(void) {
    emuFrameAccum += GetAndResetVRCBCount();
    //Stop if time hasn't passed
    if (diagTimer < DIAG_TIME) {
        return;
    }
    emuFps = emuFrameAccum / DIAG_TIME;
    emuFrameAccum = 0;
}

//Get emulator FPS
float Var_GetEmuFps(void) {
    return emuFps;
}

//Get power off variable
bool Var_GetPowerOff(void) {
    return powerOff;
}

//Set power off variable
void Var_SetPowerOff(bool value) {
    powerOff = value;
}
