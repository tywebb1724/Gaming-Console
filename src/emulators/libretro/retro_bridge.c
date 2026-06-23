#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include "libretro.h"
#include "raylib.h"
#include <string.h>
#include <sys/stat.h> // Make sure this is included

#include <limits.h>
#include <unistd.h>

#ifndef RETRO_ENVIRONMENT_SET_SYSTEM_DIRECTORY
#define RETRO_ENVIRONMENT_SET_SYSTEM_DIRECTORY 16
#endif

// External dependencies from your main Raylib application
extern Texture2D emulator_texture;
extern int your_key_map[RETRO_DEVICE_ID_JOYPAD_MASK];

// Core function pointers (Prefixed with core_ to avoid naming collisions)
void (*core_init)(void);
void (*core_deinit)(void);
unsigned (*core_api_version)(void);
void (*core_get_system_info)(struct retro_system_info *info);
bool (*core_load_game)(const struct retro_game_info *info);
void (*core_run)(void);
void (*core_unload_game)(void);

// Memory / Battery function pointers
void* (*core_get_memory_data)(unsigned id);
size_t (*core_get_memory_size)(unsigned id);

// Core setter function pointers
void (*core_set_environment)(retro_environment_t);
void (*core_set_video_refresh)(retro_video_refresh_t);
void (*core_set_audio_sample_batch)(retro_audio_sample_batch_t);
void (*core_set_input_poll)(retro_input_poll_t);
void (*core_set_input_state)(retro_input_state_t);

// --- Helper for safe dynamic loading ---
static void* GetSymbol(void* handle, const char* name) {
    void* sym = dlsym(handle, name);
    if (!sym) {
        fprintf(stderr, "Libretro Bridge: Missing symbol: %s\n", name);
    }
    return sym;
}

static struct retro_game_info current_game_info = {0};

typedef void (*retro_set_controller_port_device_t)(unsigned, unsigned);
retro_set_controller_port_device_t core_set_controller_port_device;

// --- Libretro Callbacks ---

// Call this right AFTER LoadGame() successfully runs in main.c
void LoadBattery(const char* rom_path) {
    if (!core_get_memory_data || !core_get_memory_size) return;

    // Generate a save filename by replacing the extension with .srm
    char save_path[512];
    snprintf(save_path, sizeof(save_path), "%s.srm", rom_path);

    void* core_save_ram = core_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    size_t save_size = core_get_memory_size(RETRO_MEMORY_SAVE_RAM);

    // Only load if the core actually supports save memory and a file exists
    if (core_save_ram && save_size > 0) {
        FILE* file = fopen(save_path, "rb");
        if (file) {
            fread(core_save_ram, 1, save_size, file);
            fclose(file);
            printf("Loaded save game data from: %s\n", save_path);
        }
    }
}

// Call this right BEFORE unloading the core or closing the loop in main.c
void SaveBattery(const char* rom_path) {
    if (!core_get_memory_data || !core_get_memory_size) return;

    char save_path[512];
    snprintf(save_path, sizeof(save_path), "%s.srm", rom_path);

    void* core_save_ram = core_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    size_t save_size = core_get_memory_size(RETRO_MEMORY_SAVE_RAM);

    if (core_save_ram && save_size > 0) {
        FILE* file = fopen(save_path, "wb");
        if (file) {
            fwrite(core_save_ram, 1, save_size, file);
            fclose(file);
            printf("Saved game progress to: %s\n", save_path);
        }
    }
}

// Keep track of the pixel format the core decides to use
static enum retro_pixel_format global_pixel_fmt = RETRO_PIXEL_FORMAT_RGB565;

static void video_refresh_cb(const void *data, unsigned width, unsigned height, size_t pitch) {
    if (data == NULL) {
        // A NULL pointer means the core skipped a frame or is doing hardware rendering
        // If you see this spamming, the core isn't generating software pixels
        printf("Video Callback: NULL frame received\n");
        return;
    }
    
    printf("Video Callback: Received frame %u x %u\n", width, height);
    
    if (!data || width == 0 || height == 0) return;

    // 1. Ensure our Raylib texture matches the emulator's resolution
    if (emulator_texture.width != (int)width || emulator_texture.height != (int)height) {
        UnloadTexture(emulator_texture);
        Image blank = GenImageColor(width, height, BLACK);
        blank.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8; 
        emulator_texture = LoadTextureFromImage(blank);
        UnloadImage(blank);

        // FIX HERE: Force the newly created texture to stay crisp and pixelated!
        SetTextureFilter(emulator_texture, TEXTURE_FILTER_POINT);
    }
    // 2. Allocate a temporary 32-bit RGBA buffer to translate the pixels safely
    uint32_t *converted_pixels = malloc(width * height * sizeof(uint32_t));
    if (!converted_pixels) return;

    uint8_t *src_bytes = (uint8_t*)data;

    // 3. Loop through every single pixel, respecting the row "pitch"
    for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {
            uint32_t target_pixel = 0;

            if (global_pixel_fmt == RETRO_PIXEL_FORMAT_RGB565) {
                // Convert 16-bit RGB565 to 32-bit RGBA
                uint16_t raw_pixel = *(uint16_t*)(src_bytes + (y * pitch) + (x * sizeof(uint16_t)));
                uint8_t r = ((raw_pixel >> 11) & 0x1F) * 255 / 31;
                uint8_t g = ((raw_pixel >> 5)  & 0x3F) * 255 / 63;
                uint8_t b = (raw_pixel         & 0x1F) * 255 / 31;
                target_pixel = (0xFF << 24) | (b << 16) | (g << 8) | r; // RGBA8888 format
            } 
            else if (global_pixel_fmt == RETRO_PIXEL_FORMAT_XRGB8888) {
                // Convert 32-bit XRGB8888 to 32-bit RGBA
                uint32_t raw_pixel = *(uint32_t*)(src_bytes + (y * pitch) + (x * sizeof(uint32_t)));
                uint8_t r = (raw_pixel >> 16) & 0xFF;
                uint8_t g = (raw_pixel >> 8)  & 0xFF;
                uint8_t b = raw_pixel         & 0xFF;
                target_pixel = (0xFF << 24) | (b << 16) | (g << 8) | r;
            }

            converted_pixels[y * width + x] = target_pixel;
        }
    }

    // 4. Send our beautifully converted, perfectly clean buffer to Raylib
    UpdateTexture(emulator_texture, converted_pixels);
    free(converted_pixels);
}

// 2. Input Callbacks
static void input_poll_cb(void) {
    // Left blank intentionally; handled internally by Raylib
}

static int16_t input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id) {
    if (port == 0 && device == RETRO_DEVICE_JOYPAD) {
        return IsKeyDown(your_key_map[id]) ? 1 : 0;
    }
    return 0;
}

// Even if you do nothing with the audio, you MUST return the number of frames requested
static size_t audio_sample_batch_cb(const int16_t *data, size_t frames) {
    return frames; // This tells melonDS "I took the audio, keep running!"
}

static void audio_sample_cb(int16_t left, int16_t right) {
    // Optional single-sample callback, usually safe to leave empty
}

// 4. Environment Callback
static bool retro_environment_cb(unsigned cmd, void *data) {
    printf("DEBUG: Environment cmd received: %u\n", cmd);
    switch (cmd) {
        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: {
            const enum retro_pixel_format *format = (const enum retro_pixel_format *)data;
            global_pixel_fmt = *format; // Update this global variable!
            return true;
        }
        case RETRO_ENVIRONMENT_SET_SYSTEM_DIRECTORY: {
            const char **dir = (const char **)data;
            // Point the core to your system folder
            *dir = "/home/tywebb1724/Desktop/Gaming-Console/assets/system";
            printf("SYSTEM DIR: Core instructed to use %s\n", *dir);
            return true;
        }
        case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: {
    const char *my_dir = "/home/tywebb1724/Desktop/Gaming-Console/assets/system/";
    *(const char**)data = my_dir;
    printf("DEBUG: Core requested system dir. Providing: %s\n", my_dir);
    return true;
        }
        case RETRO_ENVIRONMENT_GET_VARIABLE: {
            struct retro_variable *var = (struct retro_variable *)data;
            if (var && var->key) {
                // 1. Force Direct Boot to bypass the internal DS menu
                if (strstr(var->key, "boot_direct") != NULL || 
                    strstr(var->key, "boot_directly") != NULL) {
                    var->value = "enabled";
                    return true;
                }
                // 2. Explicitly force standard DS mode (Not DSi)
                if (strcmp(var->key, "melonds_console_mode") == 0) {
                    var->value = "DS";
                    return true;
                }
                // 3. Set a default screen layout option (often parsed as an index or mode string)
                if (strcmp(var->key, "melonds_screen_layout") == 0) {
                    var->value = "Natural";
                    return true;
                }
                // 4. Provide a valid string value for thread configuration to satisfy stoi conversions
                if (strcmp(var->key, "melonds_threaded_renderer") == 0) {
                    var->value = "disabled";
                    return true;
                }
                // 5. Disable fast memory hacks to keep the memory map simple
                if (strcmp(var->key, "melonds_jit_fastmem") == 0) {
                    var->value = "disabled";
                    return true;
                }
            }
            // Safe fallback: return false for everything else so it uses native defaults
            return false; 
        }
        
        default:
            // For general structural commands we don't support, safe false is fine
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

    // 1. Map ALL function pointers with distinct local names
    core_set_environment = (void (*)(retro_environment_t))GetSymbol(handle, "retro_set_environment");
    core_set_video_refresh = (void (*)(retro_video_refresh_t))GetSymbol(handle, "retro_set_video_refresh");
    core_set_audio_sample_batch = (void (*)(retro_audio_sample_batch_t))GetSymbol(handle, "retro_set_audio_sample_batch");
    core_set_input_poll = (void (*)(retro_input_poll_t))GetSymbol(handle, "retro_set_input_poll");
    core_set_input_state = (void (*)(retro_input_state_t))GetSymbol(handle, "retro_set_input_state");
    
    core_init = (void (*)(void))GetSymbol(handle, "retro_init");
    core_deinit = (void (*)(void))GetSymbol(handle, "retro_deinit");
    core_api_version = (unsigned (*)(void))GetSymbol(handle, "retro_api_version");
    core_get_system_info = (void (*)(struct retro_system_info *))GetSymbol(handle, "retro_get_system_info");
    core_load_game = (bool (*)(const struct retro_game_info *))GetSymbol(handle, "retro_load_game");
    core_run = (void (*)(void))GetSymbol(handle, "retro_run");

    // Map the Battery Memory function pointers dynamically
    core_get_memory_data = (void* (*)(unsigned))GetSymbol(handle, "retro_get_memory_data");
    core_get_memory_size = (size_t (*)(unsigned))GetSymbol(handle, "retro_get_memory_size");

    core_set_controller_port_device = (retro_set_controller_port_device_t)GetSymbol(handle, "retro_set_controller_port_device");

    // Safety check using our renamed variables
    if (!core_set_environment || !core_init || !core_load_game || !core_run) {
        fprintf(stderr, "Core is missing critical functions.\n");
        dlclose(handle);
        return false;
    }

    // 2. Pass your callbacks to the core
    core_set_environment(retro_environment_cb);
    core_set_video_refresh(video_refresh_cb);
    core_set_audio_sample_batch(audio_sample_batch_cb);
    core_set_input_poll(input_poll_cb);
    core_set_input_state(input_state_cb);

    // 3. Now, the core is fully configured. Initialize it.
    core_init();

    if (core_set_controller_port_device) {
    // 0 = Port 1, 1 = Classic Controller / Gamepad
    core_set_controller_port_device(0, 1); 
}
    
    return true;
}

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

bool LoadGame(const char* path) {
    if (!core_load_game) return false;

    // Use a static buffer for the path to ensure it doesn't vanish
    static char resolved_path[PATH_MAX];
    if (realpath(path, resolved_path) == NULL) {
        return false;
    }

    current_game_info.path = resolved_path;
    current_game_info.data = NULL;
    current_game_info.size = 0;

    printf("DEBUG: Loading with persistent info struct: %s\n", current_game_info.path);
    
    // Pass the address of the persistent struct
    return core_load_game(&current_game_info);
}

void RunFrame(bool is_game_running) {
    // Only call core_run if the pointer exists AND a game is actually running
    if (is_game_running && core_run != NULL) {
        core_run();
    }
}

void CloseRetroCore(void) {
    if (core_deinit) {
        core_deinit();
    }
}