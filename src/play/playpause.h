#ifndef PLAYPAUSE_H
#define PLAYPAUSE_H

#include <stdbool.h>
#include "games.h"

//State of the in-game pause menu
typedef enum {
    PLAYPAUSE_RESUME,
    PLAYPAUSE_RESTART,
    PLAYPAUSE_CONTROLS,
    PLAYPAUSE_DIAGNOSTICS,
    PLAYPAUSE_EXIT,
    PLAYPAUSE_POWER_OFF
} PlayPauseState;

//State of game after it has been launched
typedef enum {
    PLAY_RESTART,
    PLAY_GO,
    PLAY_PAUSE,
    PLAY_RESUME,
    PLAY_EXIT
} PlayState;


//PlayPause initialization
void PlayPause_Init();
//PlayPause tick
PlayState PlayPause_Tick();

#endif