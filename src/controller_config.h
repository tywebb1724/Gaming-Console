#ifndef CONTROLLER_CONFIG_H
#define CONTROLLER_CONFIG_H

#include "controller.h"

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


#endif