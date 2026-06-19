#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

// Emulator Config
typedef struct {
    uint32_t window_width;
    uint32_t window_height;
    uint32_t fg_color;
    uint32_t bg_color;
    uint32_t scaling_factor;
    bool outline;
    uint32_t IPS; // instructions per second
    // bool legacy_fx55_fx65_I_behavior; // Optional: for configurable I behavior
} config_t;

// Instruction Structure
typedef struct {
    uint16_t opcode;
    uint16_t NNN; // 12 bit address constant
    uint8_t NN;   // 8 bit constant
    uint8_t N;    // 4 bit constant (nibble)
    uint8_t X;    // 4 bit register identifier
    uint8_t Y;    // 4 bit register identifier
} instruction_t;

// Emulator States
typedef enum {
    QUIT,
    RUNNING,
    PAUSED,
} emulator_state_t;

// Chip-8 Machine State
typedef struct {
    emulator_state_t state;
    uint8_t V[16];          // General purpose registers V0-VF
    uint16_t I;             // Index register
    uint16_t PC;            // Program Counter
    uint16_t stack[12];     // Stack (CHIP-8 typically has 16 levels, 12 is a common modern choice)
    uint16_t *stack_pointer; // Stack Pointer
    bool keypad[16];        // Keypad state (16 keys)
    uint8_t ram[4096];      // Memory (4KB)
    bool display[64*32];    // Display buffer (64x32 pixels)
    uint8_t delay_timer;    // Decrements at 60Hz if > 0
    uint8_t sound_timer;    // Decrements at 60Hz if > 0, buzzes when it reaches 0
    instruction_t instruction; // Current decoded instruction
    const char *rom_name;   // Name of the loaded ROM
    int is_sound_currently_active;
} chip8_t;


// Initialize Chip-8 machine state
bool init_chip8(chip8_t *chip8, const char rom_name[]);
// Emulate one CHIP-8 instruction
void emulate_instr(chip8_t *chip8, const config_t config);

#endif