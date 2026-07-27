#ifndef PLAY_H
#define PLAY_H

#include "games.h"

//Time it takes to resume
#define RESUME_TIME 1.0f
//Drawing resume text
#define RESUME_TXT "RESUMING..."
#define RESUME_SIZE ((SCREEN_W / 160.0f) * 3.0f)
#define RESUME_SPACE (SCREEN_W / 400.0f)
#define RESUME_X (SCREEN_W / 10.0f)
#define RESUME_Y (SCREEN_H / 8.0f)
#define RESUME_RECT_X (RESUME_X - SCREEN_W / 80.0f)
#define RESUME_RECT_Y (RESUME_Y - SCREEN_H / 80.0f)
#define RESUME_RECT_W (resumeSize.x + SCREEN_W / 40.0f)
#define RESUME_RECT_H (resumeSize.y + SCREEN_H / 40.0f)

//Play initialization
void Play_Init(game_t game);
//Refresh the game
bool Play_Tick(game_t game);


#endif