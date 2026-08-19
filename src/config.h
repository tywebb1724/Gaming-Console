#ifndef CONFIG_H
#define CONFIG_H

#include "var.h"

//FPS
#define FPS 60
//Background color
#define BACKGROUND_CLR Var_GetColor2()
//Direction/position macros
#define CENTER_X (Var_GetMonitorWidth() / 2)
#define LEFT 0
#define RIGHT 1
//Max and min brightness values
#define MAX_BRIGHTNESS 0
#define MIN_BRIGHTNESS 200
//Time on a selected section before moving to the next one
#define SELECT_TIME 0.2f
//Macros for scrolling sections
#define MAX_TIME_ELAPSED 1000000
#define THRESHOLD_TIME_ELAPSED 0.25f

#define DISPLAY_CENTER_OFFSET 2

#endif