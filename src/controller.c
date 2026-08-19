#include "controller.h"
#include "raylib.h"
#include <string.h>

//Gamepad slots that are currently connected, packed low to high
static int slots[CONTROLLER_MAX];

//How many entries of slots[] are valid
static int count = 0;
//Variables for tracking whether certain buttons were already pressed
static bool wasPressed_A = false;
static bool wasPressed_B = false;
static bool wasPressed_Home = false;

//Get whether A button was pressed
bool Controller_GetWasPressed_A(void) {
    return wasPressed_A;
}

//Get whether B button was pressed
bool Controller_GetWasPressed_B(void) {
    return wasPressed_B;
}

//Get whether home button was pressed
bool Controller_GetWasPressed_Home(void) {
    return wasPressed_Home;
}

//Set whether A button was pressed
void Controller_SetWasPressed_A(bool value) {
    wasPressed_A = value;
}

//Set whether B button was pressed
void Controller_SetWasPressed_B(bool value) {
    wasPressed_B = value;
}

//Set whether home button was pressed
void Controller_SetWasPressed_Home(bool value) {
    wasPressed_Home = value;
}

//Rescan which gamepad slots are connected
void Controller_Refresh(void) {
    count = 0;
    //Go through all gamepads
    for (int i = 0; i < CONTROLLER_MAX; i++) {
        //If gamepad is connected in that slot
        if (IsGamepadAvailable(i)) {
            const char *name = GetGamepadName(i);
            //Only count Xbox controllers
            if (name && (strstr(name, "GameSir") || strstr(name, "Xbox") ||
                         strstr(name, "X-Box")   || strstr(name, "Microsoft") ||
                         strstr(name, "XInput"))) {
                slots[count] = i;
                count++;
            }
        }
    }
}

//How many controllers are connected right now
int Controller_Count(void) {
    return count;
}

//Get the slot belonging to a player
int Controller_Slot(int player) {
    //No controller for this player
    if (player < 0 || player >= count) {
        return -1;
    }
    return slots[player];
}

//Was a button just pressed on any controller
bool Controller_AnyPressed(int button) {
    //Go through all controllers
    for (int i = 0; i < count; i++) {
        //Check if any of them pressed that button
        if (IsGamepadButtonPressed(slots[i], button)) {
            return true;
        }
    }
    return false;
}

//Is a button held on any controller
bool Controller_AnyDown(int button) {
    //Go through all controllers
    for (int i = 0; i < count; i++) {
        //Check if any is holding down that button
        if (IsGamepadButtonDown(slots[i], button)) {
            return true;
        }
    }
    return false;
}

//Find the controller whose left stick is pushed the furthest from center
static int Controller_StickSlot(void) {
    int best = -1;
    float bestMag = 0.0f;
    //Go through all controllers
    for (int i = 0; i < count; i++) {
        //Get axis movement of left stick in y and x directions
        float x = GetGamepadAxisMovement(slots[i], GAMEPAD_AXIS_LEFT_X);
        float y = GetGamepadAxisMovement(slots[i], GAMEPAD_AXIS_LEFT_Y);
        float mag = x * x + y * y;
        //If the magnitude is better than the current best, replace the best
        if (mag > bestMag) {
            bestMag = mag;
            best = slots[i];
        }
    }
    return best;
}

//Left stick X of the most-deflected controller
float Controller_LeftStickX(void) {
    int slot = Controller_StickSlot();
    //If invalid result, return 0
    if (slot < 0) {
        return 0.0f;
    }
    return GetGamepadAxisMovement(slot, GAMEPAD_AXIS_LEFT_X);
}

//Left stick Y of the most-deflected controller
float Controller_LeftStickY(void) {
    int slot = Controller_StickSlot();
    //If invalid result, return 0
    if (slot < 0) {
        return 0.0f;
    }
    return GetGamepadAxisMovement(slot, GAMEPAD_AXIS_LEFT_Y);
}