#ifndef STATES_H
#define STATES_H

typedef enum {
    STATE_BOOT,
    STATE_MAIN_MENU,
    STATE_APP_LAUNCHER,
    STATE_DIAGNOSTICS
} ConsoleState;

typedef enum {
    CONSOLES,
    GAMES
} MenuState;

extern MenuState currentMenuState;

//Initialize the states
void State_Init(void);
//Update states and variabels and draw the correct screen
void State_UpdateAndDraw(void);

#endif