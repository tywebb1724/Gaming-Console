#ifndef PLAY_H
#define PLAY_H

#include "games.h"

//Time till kill is escalated
#define KILL_TIME 3.0f
//Aspect ratios
#define ASPECT_GB (10.f / 9)
#define ASPECT_GBA (3.0f / 2)
#define ASPECT_GG (6.0f / 5)
#define ASPECT_NGPC (20.0f / 19)
#define ASPECT_OTHER (4.0f / 3) 
//Max time in one frame
#define MAX_FRAME_TIME 0.25f
//Rotation macros
#define ROTATION_90 1
#define ROTATION_270 3
//Blank game image macros
#define BLANK_GAME_TEXT_W 640
#define BLANK_GAME_TEXT_H 480
//Time between saves
#define SAVE_TIME 60.0f
//Time it takes to resume
#define RESUME_TIME 1.0f
//Drawing resume text
#define PLAY_RESUME_TXT "RESUMING..."
#define PLAY_RESUME_SIZE ((Var_GetMonitorWidth() / 160.0f) * 3.0f)
#define PLAY_RESUME_SPACE (Var_GetMonitorWidth() / 400.0f)
#define PLAY_RESUME_X (Var_GetMonitorWidth() / 10.0f)
#define PLAY_RESUME_Y (Var_GetMonitorHeight() / 8.0f)
#define PLAY_RESUME_RECT_X (PLAY_RESUME_X - Var_GetMonitorWidth() / 80.0f)
#define PLAY_RESUME_RECT_Y (PLAY_RESUME_Y - Var_GetMonitorHeight() / 80.0f)
#define PLAY_RESUME_RECT_W (resumeSize.x + Var_GetMonitorWidth() / 40.0f)
#define PLAY_RESUME_RECT_H (resumeSize.y + Var_GetMonitorHeight() / 40.0f)

//Get whether doom core is active
bool Play_IsDoomActive(void);
//Get whether N64 core is active
bool Play_IsN64Active(void);
//Stop the libretro game
void Play_StopLib(const game_t* game);
//Play initialization
void Play_Init(const game_t* game);
//Play external game tick function
bool Play_TickExt(const game_t* game);
//Play libretro game tick function
bool Play_TickLib(const game_t* game);


#endif