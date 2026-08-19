#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdbool.h>

//Max gamepad slots
#define CONTROLLER_MAX 4
//Button macros
#define A_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
#define A_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
#define B_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
#define B_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
#define X_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
#define X_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
#define Y_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_RIGHT_FACE_UP))
#define Y_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_RIGHT_FACE_UP))
#define START_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_MIDDLE_RIGHT))
#define START_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_MIDDLE_RIGHT))
#define BACK_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_MIDDLE_LEFT))
#define BACK_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_MIDDLE_LEFT))
#define RB_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_RIGHT_TRIGGER_1))
#define RB_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_RIGHT_TRIGGER_1))
#define LB_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_LEFT_TRIGGER_1))
#define LB_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_LEFT_TRIGGER_1))
#define HOME_PRESS (Controller_AnyPressed(GAMEPAD_BUTTON_MIDDLE))
#define HOME_DOWN (Controller_AnyDown(GAMEPAD_BUTTON_MIDDLE)) 
//Joystick macros
#define LS_THRESHOLD_POS (0.3f)
#define LS_THRESHOLD_NEG (-0.3f)
#define LS_UP_HELP (Controller_LeftStickY() < LS_THRESHOLD_NEG)
#define LS_RIGHT_HELP (Controller_LeftStickX() > LS_THRESHOLD_POS)
#define LS_LEFT_HELP (Controller_LeftStickX() < LS_THRESHOLD_NEG)
#define LS_DOWN_HELP (Controller_LeftStickY() > LS_THRESHOLD_POS)
#define LS_UP (LS_UP_HELP && !LS_RIGHT_HELP && !LS_LEFT_HELP)
#define LS_RIGHT (LS_RIGHT_HELP && !LS_UP_HELP && !LS_DOWN_HELP)
#define LS_LEFT (LS_LEFT_HELP && !LS_UP_HELP && !LS_DOWN_HELP)
#define LS_DOWN (LS_DOWN_HELP && !LS_RIGHT_HELP && !LS_LEFT_HELP)

//Get whether A button was pressed
bool Controller_GetWasPressed_A(void);
//Get whether B button was pressed
bool Controller_GetWasPressed_B(void);
//Get whether home button was pressed
bool Controller_GetWasPressed_Home(void);
//Set whetehr A button was pressed
void Controller_SetWasPressed_A(bool value);
//Set whetehr B button was pressed
void Controller_SetWasPressed_B(bool value);
//Set whetehr home button was pressed
void Controller_SetWasPressed_Home(bool value);
//Rescan which gamepad slots are connected
void Controller_Refresh(void);
//How many controllers are connected right now
int Controller_Count(void);
//Raylib gamepad slot belonging to a given player
int Controller_Slot(int player);
//Was a button just pressed on any controller
bool Controller_AnyPressed(int button);
//Is a button held on any controller
bool Controller_AnyDown(int button);
//Left stick X of the most-deflected controller
float Controller_LeftStickX(void);
//Left stick Y of the most-deflected controller
float Controller_LeftStickY(void);



#endif