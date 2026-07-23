#ifndef STATES_H
#define STATES_H

//Time the boot up screen should display
#define BOOT_TIME 3.0f

//Load all game images (if not loaded already)
void State_LoadGameImages();
//Initialize the states
void State_Init();
//Update states and variabels and draw the correct screen
void State_UpdateAndDraw();

#endif