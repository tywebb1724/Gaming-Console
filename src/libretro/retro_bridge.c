#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include "libretro.h"
#include "raylib.h"
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include "rlgl.h"
#include "controller.h"
#include "play/play.h"
#include "retro_bridge.h"

#ifndef RETRO_ENVIRONMENT_SET_SYSTEM_DIRECTORY
#define RETRO_ENVIRONMENT_SET_SYSTEM_DIRECTORY 16
#endif

//External dependencies from the main Raylib application
extern Texture2D emulator_texture;
extern int your_key_map[RETRO_DEVICE_ID_JOYPAD_MASK];
extern int your_pad_map[RETRO_DEVICE_ID_JOYPAD_MASK];

unsigned game_rotation = 0;
static int vrcb_count = 0;

//Core function pointers (prefixed with core_ to avoid naming collisions)
void (*core_init)(void);
void (*core_deinit)(void);
unsigned (*core_api_version)(void);
void (*core_get_system_info)(struct retro_system_info *info);
bool (*core_load_game)(const struct retro_game_info *info);
void (*core_run)(void);
void (*core_unload_game)(void);

//Memory / battery function pointers
void* (*core_get_memory_data)(unsigned id);
size_t (*core_get_memory_size)(unsigned id);

static uint32_t *latest_frame = NULL;
static int latest_w = 0, latest_h = 0;
static bool frame_ready = false;
static void* core_handle = NULL;
static void* loaded_rom_data = NULL;
static size_t loaded_rom_size = 0;
static AudioStream retroStream = {0};
static bool audioStreamReady = false;

#define AUDIO_RING_SIZE (1 << 18)
static int16_t audioRing[AUDIO_RING_SIZE];
static volatile unsigned int ringWrite = 0;
static volatile unsigned int ringRead = 0;

static struct retro_hw_render_callback hw_render_callback = {0};

//Core setter function pointers
void (*core_set_environment)(retro_environment_t);
void (*core_set_video_refresh)(retro_video_refresh_t);
void (*core_set_audio_sample_batch)(retro_audio_sample_batch_t);
void (*core_set_audio_sample)(retro_audio_sample_t);
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

void (*core_get_system_av_info)(struct retro_system_av_info *info);

static struct retro_game_info current_game_info = {0};

typedef void (*retro_set_controller_port_device_t)(unsigned, unsigned);
retro_set_controller_port_device_t core_set_controller_port_device;

static unsigned int hw_framebuffer_id = 0;

void SetHWFramebuffer(unsigned int id) {
    hw_framebuffer_id = id;
}

// --- Libretro Callbacks ---
unsigned GetGameRotation(void) {
    return game_rotation;
}

void SetGameRotation(unsigned val) {
    game_rotation = val;
}

//Called by raylib when the stream needs more data
static void AudioStreamCallback(void *buffer, unsigned int frames) {
    int16_t *out = (int16_t *)buffer;
    unsigned int samplesNeeded = frames * 2;   // stereo

    for (unsigned int i = 0; i < samplesNeeded; i++) {
        if (ringRead != ringWrite) {
            out[i] = audioRing[ringRead & (AUDIO_RING_SIZE - 1)];
            ringRead++;
        } else {
            out[i] = 0;   // underrun: output silence rather than garbage
        }
    }
}

void StartRetroAudio(void) {
    if (!core_get_system_av_info) return;
    struct retro_system_av_info av = {0};
    core_get_system_av_info(&av);
    unsigned int sampleRate = (unsigned int)av.timing.sample_rate;
    if (sampleRate == 0) sampleRate = 44100;

    ringWrite = 0;
    ringRead = 0;

    retroStream = LoadAudioStream(sampleRate, 16, 2);
    SetAudioStreamCallback(retroStream, AudioStreamCallback);
    PlayAudioStream(retroStream);
    audioStreamReady = true;
}

void StopRetroAudio(void) {
    if (audioStreamReady) {
        StopAudioStream(retroStream);
        UnloadAudioStream(retroStream);
        audioStreamReady = false;
    }
}

static size_t audio_sample_batch_cb(const int16_t *data, size_t frames) {
    unsigned int samples = (unsigned int)(frames * 2);

    for (unsigned int i = 0; i < samples; i++) {
        unsigned int next = ringWrite + 1;
        // Only write if we won't catch up to the read pointer
        if ((next & (AUDIO_RING_SIZE - 1)) != (ringRead & (AUDIO_RING_SIZE - 1))) {
            audioRing[ringWrite & (AUDIO_RING_SIZE - 1)] = data[i];
            ringWrite = next;
        }
        // else: buffer full, drop this sample (prevents corruption)
    }
    return frames;
}

static uintptr_t get_current_framebuffer(void) {
    return (uintptr_t)hw_framebuffer_id;   // the FBO raylib created
}

static retro_proc_address_t get_proc_address(const char *sym) {
    return (retro_proc_address_t)rlGetProcAddress(sym);
}

void TriggerContextDestroy(void) {
    if (hw_render_callback.context_destroy) {
        hw_render_callback.context_destroy();
    }
}

// Call this right AFTER LoadGame() successfully runs in main.c
void LoadBattery(const char* rom_path) {
    if (!core_get_memory_data || !core_get_memory_size) return;

    char save_path[SAVE_PATH_LEN];
    snprintf(save_path, sizeof(save_path), "%s.srm", rom_path);

    void* core_save_ram = core_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    size_t save_size = core_get_memory_size(RETRO_MEMORY_SAVE_RAM);

    if (core_save_ram && save_size > 0) {
        FILE* file = fopen(save_path, "rb");
        if (file) {
            fread(core_save_ram, 1, save_size, file);
            fclose(file);
        }
    }
}

// Call this right BEFORE unloading the core or closing the loop in main.c
void SaveBattery(const char* rom_path) {
    if (!core_get_memory_data || !core_get_memory_size) return;

    char save_path[SAVE_PATH_LEN];
    snprintf(save_path, sizeof(save_path), "%s.srm", rom_path);

    void* core_save_ram = core_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    size_t save_size = core_get_memory_size(RETRO_MEMORY_SAVE_RAM);

    if (core_save_ram && save_size > 0) {
        FILE* file = fopen(save_path, "wb");
        if (file) {
            fwrite(core_save_ram, 1, save_size, file);
            fclose(file);
        }
    }
}

// Keep track of the pixel format the core decides to use
static volatile enum retro_pixel_format global_pixel_fmt = RETRO_PIXEL_FORMAT_RGB565;

int GetAndResetVRCBCount(void) {
    int c = vrcb_count;
    vrcb_count = 0;
    return c;
}

void PresentFrame(void) {
    if (!frame_ready || !latest_frame) return;

    if (emulator_texture.width != latest_w || emulator_texture.height != latest_h) {
        UnloadTexture(emulator_texture);
        Image blank = GenImageColor(latest_w, latest_h, BLACK);
        blank.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        emulator_texture = LoadTextureFromImage(blank);
        UnloadImage(blank);
        SetTextureFilter(emulator_texture, TEXTURE_FILTER_POINT);
    }

    UpdateTexture(emulator_texture, latest_frame);   // the ONE upload per displayed frame
    frame_ready = false;
}

static void video_refresh_cb(const void *data, unsigned width, unsigned height, size_t pitch) {
    vrcb_count++;

    if (data == RETRO_HW_FRAME_BUFFER_VALID) {
        printf("DEBUG: HW framebuffer path, w=%u h=%u\n", width, height);
        latest_w = (int)width;
        latest_h = (int)height;
        frame_ready = true;
        return;   // absolutely nothing else — no free, no malloc, no latest_frame
    }

    if (data == NULL) {
        printf("DEBUG: NULL data — dupe frame\n");
        return;
    }

    printf("DEBUG: software path, w=%u h=%u pitch=%zu fmt=%d\n", width, height, pitch, global_pixel_fmt);

    // (Re)allocate the persistent buffer only if the size changed
    if (latest_w != (int)width || latest_h != (int)height) {
        free(latest_frame);
        latest_frame = malloc(width * height * sizeof(uint32_t));
        latest_w = width;
        latest_h = height;
    }
    if (!latest_frame) return;

    uint8_t *src_bytes = (uint8_t*)data;

    for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {
            uint32_t target_pixel = 0;

            if (global_pixel_fmt == RETRO_PIXEL_FORMAT_RGB565) {
                uint16_t raw_pixel = *(uint16_t*)(src_bytes + (y * pitch) + (x * sizeof(uint16_t)));
                uint8_t r = ((raw_pixel >> 11) & 0x1F) * 255 / 31;
                uint8_t g = ((raw_pixel >> 5)  & 0x3F) * 255 / 63;
                uint8_t b = (raw_pixel         & 0x1F) * 255 / 31;
                target_pixel = (0xFF << 24) | (b << 16) | (g << 8) | r;
            }
            else if (global_pixel_fmt == RETRO_PIXEL_FORMAT_XRGB8888) {
                uint32_t raw_pixel = *(uint32_t*)(src_bytes + (y * pitch) + (x * sizeof(uint32_t)));
                uint8_t r = (raw_pixel >> 16) & 0xFF;
                uint8_t g = (raw_pixel >> 8)  & 0xFF;
                uint8_t b = raw_pixel         & 0xFF;
                target_pixel = (0xFF << 24) | (b << 16) | (g << 8) | r;
            }
            else {
                // 0RGB1555 fallback
                uint16_t raw_pixel = *(uint16_t*)(src_bytes + (y * pitch) + (x * sizeof(uint16_t)));
                uint8_t r = ((raw_pixel >> 10) & 0x1F) * 255 / 31;
                uint8_t g = ((raw_pixel >> 5)  & 0x1F) * 255 / 31;
                uint8_t b = (raw_pixel         & 0x1F) * 255 / 31;
                target_pixel = (0xFF << 24) | (b << 16) | (g << 8) | r;
            }

            latest_frame[y * width + x] = target_pixel;
        }
    }

    frame_ready = true;
}

// --- Input Callbacks ---
static void input_poll_cb(void) {
    // Left blank intentionally; handled internally by Raylib
}

static int16_t input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id) {
    if (port >= CONTROLLER_MAX) return 0;

    int slot = Controller_Slot((int)port);
    bool padOK = (slot >= 0);
    bool kbdOK = (port == 0);   // keyboard only ever drives player one

    // ---- MOUSE (Doom mouse-look) ----
    if (device == RETRO_DEVICE_MOUSE) {
        if (port != 0) return 0;   // only player 1
        switch (id) {
            case RETRO_DEVICE_ID_MOUSE_X: {
                Vector2 d = GetMouseDelta();
                return (int16_t)(d.x * MOUSE_SENSITIVITY);   // turn
            }
            case RETRO_DEVICE_ID_MOUSE_Y: {
                Vector2 d = GetMouseDelta();
                return (int16_t)(d.y * MOUSE_SENSITIVITY);   // usually unused in Doom
            }
            case RETRO_DEVICE_ID_MOUSE_LEFT:
                return IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 1 : 0;   // fire
            case RETRO_DEVICE_ID_MOUSE_RIGHT:
                return IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ? 1 : 0;  // use/alt
            default:
                return 0;
        }
    }

    // ---- DIGITAL BUTTONS (JOYPAD) ----
    if (device == RETRO_DEVICE_JOYPAD) {
        int key = your_key_map[id];
        int pad = your_pad_map[id];
        if (kbdOK && key != 0 && IsKeyDown(key)) return 1;
        if (pad != 0 && padOK && IsGamepadButtonDown(slot, pad)) return 1;

        if (padOK) {
            float lx = GetGamepadAxisMovement(slot, GAMEPAD_AXIS_LEFT_X);
            float ly = GetGamepadAxisMovement(slot, GAMEPAD_AXIS_LEFT_Y);

            if (Play_IsDoomActive()) {
                // Left stick: forward/back + strafe
                if (id == RETRO_DEVICE_ID_JOYPAD_UP    && ly < -STICK_DIGITAL_THRESHOLD) return 1;
                if (id == RETRO_DEVICE_ID_JOYPAD_DOWN  && ly >  STICK_DIGITAL_THRESHOLD) return 1;
                if (id == RETRO_DEVICE_ID_JOYPAD_L     && lx < -STICK_DIGITAL_THRESHOLD) return 1;  // strafe left
                if (id == RETRO_DEVICE_ID_JOYPAD_R     && lx >  STICK_DIGITAL_THRESHOLD) return 1;  // strafe right

                // Right stick: turn
                float rx = GetGamepadAxisMovement(slot, GAMEPAD_AXIS_RIGHT_X);
                if (id == RETRO_DEVICE_ID_JOYPAD_LEFT  && rx < -STICK_DIGITAL_THRESHOLD) return 1;
                if (id == RETRO_DEVICE_ID_JOYPAD_RIGHT && rx >  STICK_DIGITAL_THRESHOLD) return 1;
            }
            else if (Play_IsN64Active()) {
                // Intentionally empty: N64's D-pad and stick are separate inputs.
                // Real D-pad buttons come from pad_map above; stick movement
                // comes from RETRO_DEVICE_ANALOG below. No stick-as-dpad fallback.
            }
            else {
                // Everything else: left stick as dpad
                if (id == RETRO_DEVICE_ID_JOYPAD_LEFT  && lx < -STICK_DIGITAL_THRESHOLD) return 1;
                if (id == RETRO_DEVICE_ID_JOYPAD_RIGHT && lx >  STICK_DIGITAL_THRESHOLD) return 1;
                if (id == RETRO_DEVICE_ID_JOYPAD_UP    && ly < -STICK_DIGITAL_THRESHOLD) return 1;
                if (id == RETRO_DEVICE_ID_JOYPAD_DOWN  && ly >  STICK_DIGITAL_THRESHOLD) return 1;
            }
        }
        return 0;
    }

    // ---- LEFT ANALOG STICK (movement for N64/PS1) ----
    if (device == RETRO_DEVICE_ANALOG && index == RETRO_DEVICE_INDEX_ANALOG_LEFT) {
        if (id == RETRO_DEVICE_ID_ANALOG_X) {
            if (kbdOK) {
                if (Play_IsN64Active()) {
                    if (IsKeyDown(KEY_A)) return -ANALOG_MAX;
                    if (IsKeyDown(KEY_D)) return  ANALOG_MAX;
                } else {
                    if (IsKeyDown(KEY_LEFT))  return -ANALOG_MAX;
                    if (IsKeyDown(KEY_RIGHT)) return  ANALOG_MAX;
                }
            }
            if (padOK) {
                float v = GetGamepadAxisMovement(slot, GAMEPAD_AXIS_LEFT_X);
                if (v < -ANALOG_DEADZONE || v > ANALOG_DEADZONE) return (int16_t)(v * ANALOG_MAX);
            }
            return 0;
        }
        if (id == RETRO_DEVICE_ID_ANALOG_Y) {
            if (kbdOK) {
                if (Play_IsN64Active()) {
                    if (IsKeyDown(KEY_W)) return -ANALOG_MAX;
                    if (IsKeyDown(KEY_S)) return  ANALOG_MAX;
                } else {
                    if (IsKeyDown(KEY_UP))   return -ANALOG_MAX;
                    if (IsKeyDown(KEY_DOWN)) return  ANALOG_MAX;
                }
            }
            if (padOK) {
                float v = GetGamepadAxisMovement(slot, GAMEPAD_AXIS_LEFT_Y);
                if (v < -ANALOG_DEADZONE || v > ANALOG_DEADZONE) return (int16_t)(v * ANALOG_MAX);
            }
            return 0;
        }
        return 0;
    }

    // ---- RIGHT ANALOG STICK (N64 C-buttons / PS1 right stick) ----
    if (device == RETRO_DEVICE_ANALOG && index == RETRO_DEVICE_INDEX_ANALOG_RIGHT) {
        if (id == RETRO_DEVICE_ID_ANALOG_X) {
            if (kbdOK && IsKeyDown(KEY_J)) return -ANALOG_MAX;
            if (kbdOK && IsKeyDown(KEY_L)) return  ANALOG_MAX;
            if (padOK) {
                float v = GetGamepadAxisMovement(slot, GAMEPAD_AXIS_RIGHT_X);
                if (v < -ANALOG_DEADZONE || v > ANALOG_DEADZONE) {
                    float scaled = v * ANALOG_MAX * RIGHT_STICK_TURN_SENSITIVITY;
                    if (scaled >  ANALOG_MAX) scaled =  ANALOG_MAX;
                    if (scaled < -ANALOG_MAX) scaled = -ANALOG_MAX;
                    return (int16_t)scaled;
                }
            }
            return 0;
        }
        if (id == RETRO_DEVICE_ID_ANALOG_Y) {
            if (kbdOK && IsKeyDown(KEY_I)) return -ANALOG_MAX;   // C-up
            if (kbdOK && IsKeyDown(KEY_K)) return  ANALOG_MAX;   // C-down
            if (padOK) {
                float v = GetGamepadAxisMovement(slot, GAMEPAD_AXIS_RIGHT_Y);
                if (v < -ANALOG_DEADZONE || v > ANALOG_DEADZONE) return (int16_t)(v * ANALOG_MAX);
            }
            return 0;
        }
        return 0;
    }

    return 0;
}

static void audio_sample_cb(int16_t left, int16_t right) {
    audioRing[ringWrite & (AUDIO_RING_SIZE - 1)] = left;
    ringWrite++;
    audioRing[ringWrite & (AUDIO_RING_SIZE - 1)] = right;
    ringWrite++;
}

void TriggerContextReset(void) {
    if (hw_render_callback.context_reset) {
        hw_render_callback.context_reset();
    }
}

static void my_custom_logger(enum retro_log_level level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("LIBRETRO-LOG: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

// --- Environment Callback ---
static bool retro_environment_cb(unsigned cmd, void *data) {
    switch (cmd) {
        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: {
            const enum retro_pixel_format *fmt = (const enum retro_pixel_format *)data;
            global_pixel_fmt = *fmt;
            return true;
        }

        case RETRO_ENVIRONMENT_GET_LOG_INTERFACE: {
            struct retro_log_callback *log_cb = (struct retro_log_callback *)data;
            log_cb->log = my_custom_logger;
            return true;
        }

        case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY: {
            *(const char**)data = "assets/saves";
            return true;
        }

        case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: {
            const char *my_dir = "assets/system/";
            *(const char**)data = my_dir;
            return true;
        }

        case RETRO_ENVIRONMENT_SET_MESSAGE:
            return true;

        case RETRO_ENVIRONMENT_SET_HW_RENDER: {
            struct retro_hw_render_callback *hw = (struct retro_hw_render_callback *)data;
            hw_render_callback = *hw;   // store core's callbacks
            hw->get_current_framebuffer = get_current_framebuffer;
            hw->get_proc_address        = get_proc_address;
            return true;
        }

        case RETRO_ENVIRONMENT_SET_ROTATION: {
            unsigned rot = *(const unsigned*)data;
            game_rotation = rot;   // 0-3 = number of 90° CCW turns
            return true;
        }

        case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE: {
            // Allows the core to tell the menu it wants to eject/change the disc.
            // Store this pointer if disc-swap support is added later.
            return true;
        }

        case RETRO_ENVIRONMENT_GET_VARIABLE: {
            struct retro_variable *var = (struct retro_variable *)data;

            // Force the CPU to 'Interpreter' to bypass the mprotect crash
            if (strcmp(var->key, "flycast_cpu_mode") == 0) {
                var->value = "interpreter";
                return true;
            }
            // Disable HLE BIOS to force retail files
            if (strcmp(var->key, "flycast_hle") == 0) {
                var->value = "disabled";
                return true;
            }
            if (strcmp(var->key, "swanstation_MemoryCards_Card1Type") == 0) {
                var->value = "PerGame";   // persistent per-game memory card file
                return true;
            }
            if (strcmp(var->key, "gambatte_gb_bootloader") == 0) {
                var->value = "disabled";  // don't look for BIOS files
                return true;
            }
            if (strcmp(var->key, "genesis_plus_gx_cart_size") == 0) {
                var->value = "disabled";
                return true;
            }
            if (strcmp(var->key, "parallel-n64-gliden64-EnableFBEmulation") == 0) {
                var->value = "False";   // force framebuffer emulation off
                return true;
            }
            if (strcmp(var->key, "genesis_plus_gx_system_bram") == 0) {
                var->value = "per game";
                return true;
            }
            if (strcmp(var->key, "genesis_plus_gx_cart_bram") == 0) {
                var->value = "per game";   // per-game save file for the RAM cart
                return true;
            }
            if (strcmp(var->key, "genesis_plus_gx_psg_preamp") == 0)    { var->value = "150"; return true; }
            if (strcmp(var->key, "genesis_plus_gx_fm_preamp") == 0)     { var->value = "100"; return true; }
            if (strcmp(var->key, "genesis_plus_gx_sound_output") == 0)  { var->value = "stereo"; return true; }
            if (strcmp(var->key, "genesis_plus_gx_ym2612") == 0)        { var->value = "mame (ym2612)"; return true; }
            if (strcmp(var->key, "prboom-mouse_on") == 0) {
                var->value = "enabled";
                return true;
            }

            // Default: unrecognized key
            return false;
        }

        case RETRO_ENVIRONMENT_SET_VARIABLES:
            // Core telling us what variables it supports
            return true;

        case RETRO_ENVIRONMENT_GET_CAN_DUPE: {
            bool *can_dupe = (bool *)data;
            *can_dupe = true;
            return true;
        }

        default:
            return false;
    }
}

// --- Main Bridge API ---

bool LoadRetroCore(const char* path) {
    if (core_handle) {
        CloseRetroCore();   // already loaded — tear down first
    }
    core_handle = dlopen(path, RTLD_LAZY);
    if (!core_handle) {
        fprintf(stderr, "Failed to load core: %s\n", dlerror());
        return false;
    }

    core_set_environment        = (void (*)(retro_environment_t))GetSymbol(core_handle, "retro_set_environment");
    core_set_video_refresh      = (void (*)(retro_video_refresh_t))GetSymbol(core_handle, "retro_set_video_refresh");
    core_set_audio_sample_batch = (void (*)(retro_audio_sample_batch_t))GetSymbol(core_handle, "retro_set_audio_sample_batch");
    core_set_audio_sample       = (void (*)(retro_audio_sample_t))GetSymbol(core_handle, "retro_set_audio_sample");
    core_set_input_poll         = (void (*)(retro_input_poll_t))GetSymbol(core_handle, "retro_set_input_poll");
    core_set_input_state        = (void (*)(retro_input_state_t))GetSymbol(core_handle, "retro_set_input_state");

    core_init             = (void (*)(void))GetSymbol(core_handle, "retro_init");
    core_deinit           = (void (*)(void))GetSymbol(core_handle, "retro_deinit");
    core_api_version       = (unsigned (*)(void))GetSymbol(core_handle, "retro_api_version");
    core_get_system_info   = (void (*)(struct retro_system_info *))GetSymbol(core_handle, "retro_get_system_info");
    core_load_game         = (bool (*)(const struct retro_game_info *))GetSymbol(core_handle, "retro_load_game");
    core_run               = (void (*)(void))GetSymbol(core_handle, "retro_run");
    core_unload_game       = (void (*)(void))GetSymbol(core_handle, "retro_unload_game");

    core_get_memory_data = (void* (*)(unsigned))GetSymbol(core_handle, "retro_get_memory_data");
    core_get_memory_size = (size_t (*)(unsigned))GetSymbol(core_handle, "retro_get_memory_size");

    core_set_controller_port_device = (retro_set_controller_port_device_t)GetSymbol(core_handle, "retro_set_controller_port_device");

    core_get_system_av_info = (void (*)(struct retro_system_av_info *))GetSymbol(core_handle, "retro_get_system_av_info");

    if (!core_set_environment || !core_init || !core_load_game || !core_run) {
        fprintf(stderr, "Core is missing critical functions.\n");
        dlclose(core_handle);
        return false;
    }

    core_set_environment(retro_environment_cb);
    core_set_video_refresh(video_refresh_cb);
    core_set_audio_sample_batch(audio_sample_batch_cb);
    core_set_audio_sample(audio_sample_cb);
    core_set_input_poll(input_poll_cb);
    core_set_input_state(input_state_cb);

    core_init();

    if (core_set_controller_port_device) {
        // Register every port a controller could turn up on (1 = RETRO_DEVICE_JOYPAD)
        for (unsigned p = 0; p < CONTROLLER_MAX; p++) {
            core_set_controller_port_device(p, 1);
        }
    }

    return true;
}

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

bool LoadGame(const char* path) {
    if (!core_load_game || !core_get_system_info) return false;

    static char resolved_path[PATH_MAX];
    if (realpath(path, resolved_path) == NULL) {
        fprintf(stderr, "LoadGame: realpath failed for %s\n", path);
        return false;
    }

    struct retro_system_info sys_info = {0};
    core_get_system_info(&sys_info);

    current_game_info.path = resolved_path;
    current_game_info.meta = NULL;

    if (sys_info.need_fullpath) {
        // Core opens the file itself (e.g. CD-based cores like Flycast)
        current_game_info.data = NULL;
        current_game_info.size = 0;
    } else {
        // Core expects the frontend to load the ROM into memory (e.g. Gambatte)
        FILE* f = fopen(resolved_path, "rb");
        if (!f) {
            fprintf(stderr, "LoadGame: fopen failed for %s\n", resolved_path);
            return false;
        }

        if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
        long sz = ftell(f);
        if (sz <= 0) { fclose(f); return false; }
        rewind(f);

        if (loaded_rom_data) { free(loaded_rom_data); loaded_rom_data = NULL; }

        loaded_rom_data = malloc((size_t)sz);
        if (!loaded_rom_data) { fclose(f); return false; }

        size_t read = fread(loaded_rom_data, 1, (size_t)sz, f);
        fclose(f);

        if (read != (size_t)sz) {
            free(loaded_rom_data);
            loaded_rom_data = NULL;
            return false;
        }

        loaded_rom_size = (size_t)sz;
        current_game_info.data = loaded_rom_data;
        current_game_info.size = loaded_rom_size;
    }

    return core_load_game(&current_game_info);
}

void RunFrame(bool is_game_running) {
    if (is_game_running && core_run != NULL) {
        core_run();
    }
}

void CloseRetroCore(void) {
    if (core_deinit) {
        core_deinit();   // flushes BRAM
    }
    if (core_handle) {
        dlclose(core_handle);
        core_handle = NULL;
    }

    // Null all function pointers so nothing stale can be called
    core_init = NULL; core_deinit = NULL; core_run = NULL;
    core_load_game = NULL; core_unload_game = NULL;
    core_get_system_av_info = NULL; core_get_memory_data = NULL;
    core_get_memory_size = NULL; core_get_system_info = NULL;

    if (loaded_rom_data) {
        free(loaded_rom_data);
        loaded_rom_data = NULL;
        loaded_rom_size = 0;
    }

    hw_render_callback = (struct retro_hw_render_callback){0};
}

double GetCoreTargetFPS(void) {
    if (!core_get_system_av_info) return 60.0;
    struct retro_system_av_info av = {0};
    core_get_system_av_info(&av);
    double fps = av.timing.fps;
    if (fps <= 0.0) return 60.0;   // guard against a zero/garbage report
    return fps;
}