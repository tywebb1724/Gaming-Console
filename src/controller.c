#include "controller.h"
#include "raylib.h"
#include <string.h>

//Gamepad slots that are currently connected, packed low to high
static int slots[CONTROLLER_MAX];
//How many entries of slots[] are valid
static int count = 0;

static bool wasPressed_A = false;
static bool wasPressed_B = false;
static bool wasPressed_Home = false;

//Get whether A button was pressed
bool Controller_GetWasPressed_A() {
    return wasPressed_A;
}

//Get whether B button was pressed
bool Controller_GetWasPressed_B() {
    return wasPressed_B;
}

//Get whether home button was pressed
bool Controller_GetWasPressed_Home() {
    return wasPressed_Home;
}

//Set whetehr A button was pressed
void Controller_SetWasPressed_A(bool value) {
    wasPressed_A = value;
}

//Set whetehr B button was pressed
void Controller_SetWasPressed_B(bool value) {
    wasPressed_B = value;
}

//Set whetehr home button was pressed
void Controller_SetWasPressed_Home(bool value) {
    wasPressed_Home = value;
}

//Rescan which gamepad slots are connected
void Controller_Refresh(void) {
    count = 0;
    for (int i = 0; i < CONTROLLER_MAX; i++) {
        if (IsGamepadAvailable(i)) {
            const char *name = GetGamepadName(i);
            //Only count real controllers — skip the mouse/other devices raylib exposes as gamepads
            if (name && (strstr(name, "GameSir") || strstr(name, "Xbox") ||
                         strstr(name, "X-Box")   || strstr(name, "Microsoft") ||
                         strstr(name, "Controller"))) {
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
    for (int i = 0; i < count; i++) {
        if (IsGamepadButtonPressed(slots[i], button)) {
            return true;
        }
    }
    return false;
}

//Is a button held on any controller
bool Controller_AnyDown(int button) {
    for (int i = 0; i < count; i++) {
        if (IsGamepadButtonDown(slots[i], button)) {
            return true;
        }
    }
    return false;
}

//Find the controller whose left stick is pushed the furthest from centre
static int Controller_StickSlot(void) {
    int best = -1;
    float bestMag = 0.0f;
    for (int i = 0; i < count; i++) {
        float x = GetGamepadAxisMovement(slots[i], GAMEPAD_AXIS_LEFT_X);
        float y = GetGamepadAxisMovement(slots[i], GAMEPAD_AXIS_LEFT_Y);
        //Squared magnitude, so no square root is needed
        float mag = x * x + y * y;
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
    if (slot < 0) {
        return 0.0f;
    }
    return GetGamepadAxisMovement(slot, GAMEPAD_AXIS_LEFT_X);
}

//Left stick Y of the most-deflected controller
float Controller_LeftStickY(void) {
    int slot = Controller_StickSlot();
    if (slot < 0) {
        return 0.0f;
    }
    return GetGamepadAxisMovement(slot, GAMEPAD_AXIS_LEFT_Y);
}