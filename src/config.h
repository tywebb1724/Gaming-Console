#ifndef CONFIG_H
#define CONFIG_H

//FPS
#define FPS 60
//Background color
#define BACKGROUND_CLR Var_GetColor2()
//Screen width and height
#define SCREEN_W Var_GetMonitorWidth()
#define SCREEN_H Var_GetMonitorHeight()
//Direction/position macros
#define CENTER_X (SCREEN_W / 2)
#define LEFT 0
#define RIGHT 1

#define MAX_BRIGHTNESS 0
#define MIN_BRIGHTNESS 200




#endif