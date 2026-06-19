#include "chip8_app.h"
#include "raylib.h"

float chip8_ScaleX;
float chip8_ScaleY;
float chip8_Scale;
float chip8_OffsetX;
float chip8_OffsetY;


chip8_t chip8;
config_t chip8_config = { .window_width = 64, .window_height = 32 };
bool chip8_is_initialized;

// Define the mapping once here, private to this file
static const int keymap[16] = {
    KEY_X, KEY_ONE, KEY_TWO, KEY_THREE, 
    KEY_Q, KEY_W, KEY_E, KEY_A, 
    KEY_S, KEY_D, KEY_Z, KEY_C, 
    KEY_FOUR, KEY_R, KEY_F, KEY_V
};

void chip8_init(int screenWidth, int screenHeight) {
    chip8_is_initialized = false;
    // Calculate scale for width and height
    chip8_ScaleX = screenWidth / 64.0f;
    chip8_ScaleY = screenHeight / 32.0f;

    // Use the smaller scale to ensure the whole 64x32 grid fits without distortion
    chip8_Scale = (chip8_ScaleX < chip8_ScaleY) ? chip8_ScaleX : chip8_ScaleY;

    // Calculate centering offsets
    chip8_OffsetX = (screenWidth - (64 * chip8_Scale)) / 2.0f;
    chip8_OffsetY = (screenHeight - (32 * chip8_Scale)) / 2.0f;

    // 1. Setup (Initialization)
    if (!chip8_is_initialized) {
        if (init_chip8(&chip8, "roms/chip8/snake.ch8")) {
            chip8_is_initialized = true;
        }
    }
}

//Update chip8 game
void chip8_update() {
    ClearBackground(BLACK);
    // 1. Input
    for (int i = 0; i < 16; i++) {
        chip8.keypad[i] = IsKeyDown(keymap[i]);
    }

    // 2. Emulate
    for(int i = 0; i < 10; i++) {
        emulate_instr(&chip8, chip8_config);
    }

    // 3. Timers
    if (chip8.delay_timer > 0) chip8.delay_timer--;
    if (chip8.sound_timer > 0) chip8.sound_timer--;

    // 4. Render
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (chip8.display[y * 64 + x]) {
                DrawRectangle((int)(chip8_OffsetX + x * chip8_Scale), 
                          (int)(chip8_OffsetY + y * chip8_Scale), 
                          (int)chip8_Scale, 
                          (int)chip8_Scale, 
                          WHITE);
            }
        }
    }
}
