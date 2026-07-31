#ifndef PLAY_H
#define PLAY_H

#include "games.h"

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

bool Play_IsDoomActive(void);
//Stop the game
void Play_Stop(const game_t* game);
//Play initialization
void Play_Init(const game_t* game);
//Refresh the game
bool Play_Tick(const game_t* game);


#endif