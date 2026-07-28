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




#endif