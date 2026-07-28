#ifndef PLAY_H
#define PLAY_H

#include "games.h"

//Time it takes to resume
#define RESUME_TIME 1.0f
//Drawing resume text
#define RESUME_TXT "RESUMING..."
#define RESUME_SIZE ((Var_GetMonitorWidth() / 160.0f) * 3.0f)
#define RESUME_SPACE (Var_GetMonitorWidth() / 400.0f)
#define RESUME_X (Var_GetMonitorWidth() / 10.0f)
#define RESUME_Y (Var_GetMonitorHeight() / 8.0f)
#define RESUME_RECT_X (RESUME_X - Var_GetMonitorWidth() / 80.0f)
#define RESUME_RECT_Y (RESUME_Y - Var_GetMonitorHeight() / 80.0f)
#define RESUME_RECT_W (resumeSize.x + Var_GetMonitorWidth() / 40.0f)
#define RESUME_RECT_H (resumeSize.y + Var_GetMonitorHeight() / 40.0f)

//Stop the game
void Play_Stop(game_t game);
//Play initialization
void Play_Init(game_t game);
//Refresh the game
bool Play_Tick(game_t game);


#endif