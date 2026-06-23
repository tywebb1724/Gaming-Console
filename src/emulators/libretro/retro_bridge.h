#ifndef RETRO_BRIDGE_H
#define RETRO_BRIDGE_H

#include <stdbool.h>

extern void (*core_run)(void);
extern void (*core_unload_game)(void);
extern void (*core_deinit)(void);

// --- Main Bridge API ---

// Loads the dynamic library (.so or .dll) for the emulator core and resolves symbols.
bool LoadRetroCore(const char* path);

// Passes the ROM file path to the loaded core to start the game.
bool LoadGame(const char* path);

// Steps the emulator forward by exactly one frame. Call this in your main update loop.
void RunFrame(bool is_game_running);

// Destroys the emulator context and frees allocated memory upon exit.
void CloseRetroCore(void);

void LoadBattery(const char* rom_path);
void SaveBattery(const char* rom_path);

#endif // RETRO_BRIDGE_H