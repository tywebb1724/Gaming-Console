#ifndef STATES_H
#define STATES_H

//Time the boot up screen should display
#define BOOT_TIME 3.0f
//Time on launch screen
#define LAUNCH_TIME 3.0f
//Length of string variables
#define COLOR_LEN 10
#define BRIGHT_LEN 32
#define DIAG_LEN 5

//Load all game images (if not loaded already)
void States_LoadGameImages();
//Initialize the states
void States_Init();
//Update states and variabels and draw the correct screen
void States_UpdateAndDraw();

#endif