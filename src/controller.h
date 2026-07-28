#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdbool.h>

//Max gamepad slots
#define CONTROLLER_MAX 4

//Get whether A button was pressed
bool Controller_GetWasPressed_A();
//Set whetehr A button was pressed
void Controller_SetWasPressed_A(bool value);
//Rescan which gamepad slots are connected
void Controller_Refresh(void);
//How many controllers are connected right now
int Controller_Count(void);
//Raylib gamepad slot belonging to a given player
int Controller_Slot(int player);
//Button state across every connected controller
bool Controller_AnyPressed(int button);
bool Controller_AnyDown(int button);
//Left stick of whichever controller is pushed the furthest
float Controller_LeftStickX(void);
float Controller_LeftStickY(void);



#endif