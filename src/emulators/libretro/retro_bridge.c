#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include "libretro.h"
#include "raylib.h"

// External dependencies from your main Raylib application
extern Texture2D emulator_texture;
extern int your_key_map[RETRO_DEVICE_ID_JOYPAD_MASK];

// Core function pointers
void (*retro_init)(void);
void (*retro_deinit)(void);
unsigned (*retro_api_version)(void);
void (*retro_get_system_info)(struct retro_system_info *info);
bool (*retro_load_game)(const struct retro_game_info *info);
void (*retro_run)(void);

// Core setter function pointers
void (*retro_set_environment)(retro_environment_t);
void (*retro_set_video_refresh)(retro_video_refresh_t);
void (*retro_set_audio_sample_batch)(retro_audio_sample_batch_t);
void (*retro_set_input_poll)(retro_input_poll_t);
void (*retro_set_input_state)(retro_input_state_t);

// --- Helper for safe dynamic loading ---
static void* GetSymbol(void* handle, const char* name) {
    void* sym = dlsym(handle, name);
    if (!sym) {
        fprintf(stderr, "Libretro Bridge: Missing symbol: %s\n", name);
    }
    return sym;
}

// --- Libretro Callbacks ---

// 1. Video Callback
static void video_refresh_cb(const void *data, unsigned width, unsigned height, size_t pitch) {
    if (data && width > 0 && height > 0) {
        // Update your pre-existing Raylib texture
        UpdateTexture(emulator_texture, data);
    }
}

// 2. Input Callbacks
static void input_poll_cb(void) {
    // Raylib handles input states internally, so polling here is usually empty
}

static int16_t input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id) {
    if (port == 0 && device == RETRO_DEVICE_JOYPAD) {
        return IsKeyDown(your_key_map[id]) ? 1 : 0;
    }
    return 0;
}

// 3. Audio Callback
static void audio_sample_batch_cb(const int16_t *data, size_t frames) {
    // Leave blank for now until you implement Raylib UpdateAudioStream()
}

// 4. Environment Callback
static bool retro_environment_cb(unsigned cmd, void *data) {
    switch (cmd) {
        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: {
            // Request 32-bit XRGB format which plays perfectly with Raylib UpdateTexture
            enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
            *(enum retro_pixel_format *)data = fmt;
            return true;
        }
        default:
            return false;
    }
}

// --- Main Bridge API ---

bool LoadRetroCore(const char* path) {
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Failed to load core: %s\n", dlerror());
        return false;
    }

    // 1. Map ALL function pointers first
    retro_set_environment = GetSymbol(handle, "retro_set_environment");
    retro_set_video_refresh = GetSymbol(handle, "retro_set_video_refresh");
    retro_set_audio_sample_batch = GetSymbol(handle, "retro_set_audio_sample_batch");
    retro_set_input_poll = GetSymbol(handle, "retro_set_input_poll");
    retro_set_input_state = GetSymbol(handle, "retro_set_input_state");
    
    retro_init = GetSymbol(handle, "retro_init");
    retro_deinit = GetSymbol(handle, "retro_deinit");
    retro_api_version = GetSymbol(handle, "retro_api_version");
    retro_get_system_info = GetSymbol(handle, "retro_get_system_info");
    retro_load_game = GetSymbol(handle, "retro_load_game");
    retro_run = GetSymbol(handle, "retro_run");

    // Safety check to ensure core minimum requirements are mapped
    if (!retro_set_environment || !retro_init || !retro_load_game || !retro_run) {
        fprintf(stderr, "Core is missing critical functions.\n");
        return false;
    }

    // 2. Pass your callbacks to the core
    retro_set_environment(retro_environment_cb);
    retro_set_video_refresh(video_refresh_cb);
    retro_set_audio_sample_batch(audio_sample_batch_cb);
    retro_set_input_poll(input_poll_cb);
    retro_set_input_state((retro_input_state_t)input_state_cb);

    // 3. Now, the core is fully configured. Initialize it.
    retro_init();
    
    return true;
}

bool LoadGame(const char* path) {
    if (!retro_load_game) return false;
    
    struct retro_game_info info = {0};
    info.path = path;
    
    return retro_load_game(&info);
}

void RunFrame(void) {
    if (retro_run) {
        retro_run();
    }
}

void CloseRetroCore(void) {
    if (retro_deinit) {
        retro_deinit();
    }
}