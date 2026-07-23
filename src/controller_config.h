#ifndef CONTROLLER_CONFIG_H
#define CONTROLLER_CONFIG_H

//Button macros
#define A_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
#define A_DOWN (IsGamepadButtonDown(1, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
#define B_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
#define B_DOWN (IsGamepadButtonDown(1, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
#define X_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
#define X_DOWN (IsGamepadButtonDown(1, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
#define Y_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_FACE_UP))
#define Y_DOWN (IsGamepadButtonDown(1, GAMEPAD_BUTTON_RIGHT_FACE_UP))
#define START_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_MIDDLE_RIGHT))
#define START_DOWN (IsGamepadButtonDown(1, GAMEPAD_BUTTON_MIDDLE_RIGHT))
#define BACK_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_MIDDLE_LEFT))
#define BACK_DOWN (IsGamePadButtonDown(1, GAMEPAD_BUTTON_MIDDLE_LEFT))
#define RB_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))
#define RB_DOWN (IsGamepadButtonDown(1, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))
#define LB_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
#define LB_DOWN (IsGamepadButtonDown(1, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
#define HOME_PRESS (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_MIDDLE))
#define HOME_DOWN (IsGamepadButtonDown(1, GAMEPAD_BUTTON_MIDDLE))
//Joystick macros
#define LS_THRESHOLD_POS (0.3f)
#define LS_THRESHOLD_NEG (-0.3f)
#define LS_UP_HELP (GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_Y) < LS_THRESHOLD_NEG)
#define LS_RIGHT_HELP (GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_X) > LS_THRESHOLD_POS)
#define LS_LEFT_HELP (GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_X) < LS_THRESHOLD_NEG)
#define LS_DOWN_HELP (GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_Y) > LS_THRESHOLD_POS)
#define LS_UP (LS_UP_HELP && !LS_RIGHT_HELP && !LS_LEFT_HELP)
#define LS_RIGHT (LS_RIGHT_HELP && !LS_UP_HELP && !LS_DOWN_HELP)
#define LS_LEFT (LS_LEFT_HELP && !LS_UP_HELP && !LS_DOWN_HELP)
#define LS_DOWN (LS_DOWN_HELP && !LS_RIGHT_HELP && !LS_LEFT_HELP)


#endif