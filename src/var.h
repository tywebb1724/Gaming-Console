#ifndef VAR_H
#define VAR_H

#include "raylib.h"

//Time to update diagnostics
#define DIAG_TIME 1.0f
//Conversion values
#define MICRO_TO_MILLI 1000
#define DEGREE_CONVERSION 1000.0f

#define THERM_PATH_LEN 300

//Scrolling states
typedef enum {
    SCROLL_NO,
    SCROLL_LEFT,
    SCROLL_RIGHT
} ScrollState;

//States of the console
typedef enum {
    STATE_BOOT,
    STATE_MAIN_MENU,
    STATE_LAUNCHING,
    STATE_APP_LAUNCHER,
    STATE_VIEW_DIAG
} ConsoleState;

//Initialize the variables
void Var_Init();
//Get brightness value
float Var_GetBright();
//Set brightness value
void Var_SetBright(float value);
//Set brightness value
void Var_AddBright(float value);
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
//Get display diagnostics
bool Var_GetDiag();
//Set display diagnostics
void Var_SetDiag(bool value);
//Get alpha select text
float Var_GetAlphaSelect();
//Set alpha select text
void Var_SetAlphaSelect(float value);
//Set alpha select text
void Var_AddAlphaSelect(float value);
//Get the current background
Texture2D Var_GetBackground();
//Set the current background
void Var_SetBackground(Texture2D background);
//Extract a background from a color name
Texture2D Var_NametoBackground(char *c);
//Update the UI text file
void Var_UpdateUIFile();
//Get bold font
Font Var_GetFontBold();
//Get regular font
Font Var_GetFontRegular();
//Get monitor width
float Var_GetMonitorWidth();
//Get monitor height
float Var_GetMonitorHeight();
//Get whether brightness options are being displayed
bool Var_GetDisplayBright();
//Set whether brightness options are being displayed
void Var_SetDisplayBright(bool value);
//Get whether theme options are being displayed
bool Var_GetDisplayTheme();
//Set whether theme options are being displayed
void Var_SetDisplayTheme(bool value);
void Var_FindThermalZone();
//Update CPU temperature
void Var_UpdateTemp();
//Get CPU temperature
float Var_GetTemp();
//Update CPU clock speed
void Var_UpdateClock();
//Get CPU clock speed
int Var_GetClock();
//Update frame time
void Var_UpdateFrame();
//Get average frame time
float Var_GetFrameAvg();
//Get worst frame time
float Var_GetFrameWorst();
//Update emulator FPS
void Var_UpdateEmuFps(void);
//Get emulator FPS
float Var_GetEmuFps(void);
//Get power off variable
bool Var_GetPowerOff(void);
//Set power off variable
void Var_SetPowerOff(bool value);

#endif