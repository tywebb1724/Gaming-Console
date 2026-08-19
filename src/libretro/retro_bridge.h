#ifndef RETRO_BRIDGE_H
#define RETRO_BRIDGE_H

#include <stdbool.h>
#include "libretro.h"

#define SAVE_PATH_LEN 512
#define ANALOG_MAX 32767.0f
#define ANALOG_DEADZONE 0.2f
#define STICK_DIGITAL_THRESHOLD 0.5f
#define MOUSE_SENSITIVITY 1.0f
#define RIGHT_STICK_TURN_SENSITIVITY 1.5f

// --- Core function pointers (defined in retro_bridge.c) ---
extern void (*core_init)(void);
extern bool (*core_load_game)(const struct retro_game_info *info);
extern void (*core_run)(void);
extern void (*core_unload_game)(void);
extern void (*core_deinit)(void);

// --- Core lifecycle ---

// Loads the dynamic library (.so or .dll) for the emulator core and resolves symbols.
bool LoadRetroCore(const char* path);

// Passes the ROM file path to the loaded core to start the game.
bool LoadGame(const char* path);

// Steps the emulator forward by exactly one frame. Call this in the main update loop.
void RunFrame(bool is_game_running);

// Destroys the emulator context and frees allocated memory upon exit.
void CloseRetroCore(void);

// --- Audio ---
void StartRetroAudio(void);
void StopRetroAudio(void);

// --- Video / rendering ---
void PresentFrame(void);
void SetHWFramebuffer(unsigned int id);
void TriggerContextReset(void);
void TriggerContextDestroy(void);

// --- Rotation ---
unsigned GetGameRotation(void);
void SetGameRotation(unsigned val);

// --- Diagnostics ---
double GetCoreTargetFPS(void);
int GetAndResetVRCBCount(void);

// --- Save data (battery-backed RAM) ---
void LoadBattery(const char* rom_path);
void SaveBattery(const char* rom_path);

#endif // RETRO_BRIDGE_H