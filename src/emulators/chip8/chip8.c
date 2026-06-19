#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Initialize Chip-8 machine state
bool init_chip8(chip8_t *chip8, const char rom_name[]) {
    const uint32_t entry_point = 0x200; // CHIP-8 programs start at memory location 0x200
    memset(chip8, 0, sizeof(chip8_t)); // Clear all chip8 struct fields to zero/false

    // Load Fontset (0-F) into RAM (addresses 0x000-0x04F)
    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(&chip8->ram[0x000], font, sizeof(font)); // Load font starting at address 0x000

    // Load ROM into RAM
    FILE *rom_file = fopen(rom_name, "rb"); // Open ROM file in binary read mode
    if (!rom_file) {
        fprintf(stderr, "Error: Failed to open ROM: %s\n", rom_name);
        return false;
    }

    // Get ROM size
    fseek(rom_file, 0, SEEK_END);
    long rom_size = ftell(rom_file);
    const long max_rom_size = sizeof(chip8->ram) - entry_point;
    rewind(rom_file); // Go back to the beginning of the file

    if (rom_size > max_rom_size) {
        fprintf(stderr, "ROM file %s is too large. Size: %ld bytes, Max allowed: %ld bytes\n",
                  rom_name, rom_size, max_rom_size);
        fclose(rom_file);
        return false;
    }
     if (rom_size <= 0) { // Check for empty or invalid file size
        fprintf(stderr, "ROM file %s is empty or has invalid size: %ld bytes\n", rom_name, rom_size);
        fclose(rom_file);
        return false;
    }


    // Read ROM into RAM starting at the entry point
    if (fread(&chip8->ram[entry_point], 1, rom_size, rom_file) != (size_t)rom_size) { // Check bytes read
        fprintf(stderr, "Failed to read ROM %s into RAM (read %ld bytes).\n", rom_name, ftell(rom_file));
        fclose(rom_file);
        return false;
    }
    fclose(rom_file);

    // Set initial emulator state
    chip8->state = RUNNING;
    chip8->PC = entry_point;        // Start execution at 0x200
    chip8->rom_name = rom_name;
    chip8->stack_pointer = &chip8->stack[0]; // SP points to the beginning of the stack array

    // Seed random number generator once
    srand(time(NULL));

    return true;
}

// Emulate one CHIP-8 instruction
void emulate_instr(chip8_t *chip8, const config_t config) {
    // Fetch Opcode (2 bytes, big-endian)
    chip8->instruction.opcode = (chip8->ram[chip8->PC] << 8) | chip8->ram[chip8->PC + 1];
    chip8->PC += 2; // Increment Program Counter for next instruction

    // Decode Opcode into components
    chip8->instruction.NNN = chip8->instruction.opcode & 0x0FFF; // Lowest 12 bits (address)
    chip8->instruction.NN  = chip8->instruction.opcode & 0x00FF; // Lowest 8 bits (byte value)
    chip8->instruction.N   = chip8->instruction.opcode & 0x000F; // Lowest 4 bits (nibble value)
    chip8->instruction.X   = (chip8->instruction.opcode >> 8) & 0x0F; // Lower 4 bits of the high byte (Vx register index)
    chip8->instruction.Y   = (chip8->instruction.opcode >> 4) & 0x0F; // Upper 4 bits of the low byte (Vy register index)

    #ifdef DEBUG
        print_debug_info(chip8, config);
    #endif

    // Execute Opcode based on the highest 4 bits
    switch((chip8->instruction.opcode >> 12) & 0x0F) {
        case 0x00: // Miscellaneous instructions (0NNN, 00E0, 00EE)
            switch(chip8->instruction.NN) { // Check the lowest byte
                case 0xE0: // 00E0: CLS - Clear the display
                    memset(chip8->display, false, sizeof(chip8->display));
                    break;
                case 0xEE: // 00EE: RET - Return from a subroutine
                    if (chip8->stack_pointer > chip8->stack) { // Check for stack underflow
                        chip8->stack_pointer--;                 // Decrement Stack Pointer
                        chip8->PC = *chip8->stack_pointer;      // Set PC to address from stack
                    } else {
                        #ifdef DEBUG
                        fprintf(stderr, "Error: Stack underflow on RET (PC: 0x%04X)!\n", chip8->PC - 2);
                        #endif
                        chip8->state = PAUSED; // Pause on error
                    }
                    break;
                default: // 0NNN: SYS addr - Jump to a machine code routine (Ignored on modern interpreters)
                    // Typically a No-Op for emulators.
                    break;
            }
            break;

        case 0x01: // 1NNN: JP addr - Jump to location NNN
            chip8->PC = chip8->instruction.NNN;
            break;

        case 0x02: // 2NNN: CALL addr - Call subroutine at NNN
            // Check for stack overflow before pushing
            if (chip8->stack_pointer < &chip8->stack[sizeof(chip8->stack)/sizeof(chip8->stack[0])]) {
                *chip8->stack_pointer = chip8->PC; // Store current PC (which is already advanced) on stack
                chip8->stack_pointer++;            // Increment Stack Pointer
                chip8->PC = chip8->instruction.NNN; // Set PC to subroutine address NNN
            } else {
                 #ifdef DEBUG
                fprintf(stderr, "Error: Stack overflow on CALL (PC: 0x%04X, Target: 0x%03X)!\n", chip8->PC - 2, chip8->instruction.NNN);
                #endif
                chip8->state = PAUSED; // Pause on error
            }
            break;

        case 0x03: // 3XNN: SE Vx, byte - Skip next instruction if V[X] == NN
            if (chip8->V[chip8->instruction.X] == chip8->instruction.NN) {
                chip8->PC += 2; // Skip one 2-byte instruction
            }
            break;

        case 0x04: // 4XNN: SNE Vx, byte - Skip next instruction if V[X] != NN
            if (chip8->V[chip8->instruction.X] != chip8->instruction.NN) {
                chip8->PC += 2;
            }
            break;

        case 0x05: // 5XY0: SE Vx, Vy - Skip next instruction if V[X] == V[Y]
            if (chip8->instruction.N == 0) { // Ensure last nibble is 0 for valid opcode
                if (chip8->V[chip8->instruction.X] == chip8->V[chip8->instruction.Y]) {
                    chip8->PC += 2;
                }
            } else { // Invalid opcode (5XYN where N!=0)
                 #ifdef DEBUG
                fprintf(stderr, "Warning: Invalid 5XYN opcode 0x%04X (N!=0) (PC: 0x%04X)\n", chip8->instruction.opcode, chip8->PC-2);
                #endif
            }
            break;

        case 0x06: // 6XNN: LD Vx, byte - Set V[X] = NN
            chip8->V[chip8->instruction.X] = chip8->instruction.NN;
            break;

        case 0x07: // 7XNN: ADD Vx, byte - Set V[X] = V[X] + NN (VF is not affected)
            chip8->V[chip8->instruction.X] += chip8->instruction.NN;
            break;

        case 0x08: // 8XYN: Logical and arithmetic operations
            switch (chip8->instruction.N) { // Check the last nibble (N)
                case 0: // 8XY0: LD Vx, Vy - Set V[X] = V[Y]
                    chip8->V[chip8->instruction.X] = chip8->V[chip8->instruction.Y];
                    break;
                case 1: // 8XY1: OR Vx, Vy - Set V[X] = V[X] OR V[Y]
                    chip8->V[chip8->instruction.X] |= chip8->V[chip8->instruction.Y];
                    // Optional: chip8->V[0xF] = 0; // Some interpreters clear VF here
                    break;
                case 2: // 8XY2: AND Vx, Vy - Set V[X] = V[X] AND V[Y]
                    chip8->V[chip8->instruction.X] &= chip8->V[chip8->instruction.Y];
                    // Optional: chip8->V[0xF] = 0;
                    break;
                case 3: // 8XY3: XOR Vx, Vy - Set V[X] = V[X] XOR V[Y]
                    chip8->V[chip8->instruction.X] ^= chip8->V[chip8->instruction.Y];
                    // Optional: chip8->V[0xF] = 0;
                    break;
                case 4: { // 8XY4: ADD Vx, Vy - Set V[X] = V[X] + V[Y], V[F] = carry.
                    uint16_t sum = (uint16_t)chip8->V[chip8->instruction.X] + chip8->V[chip8->instruction.Y];
                    chip8->V[0xF] = (sum > 0xFF) ? 1 : 0; // Set carry flag if overflow
                    chip8->V[chip8->instruction.X] = (uint8_t)sum; // Store lowest 8 bits of sum in V[X]
                    break;
                }
                case 5: // 8XY5: SUB Vx, Vy - Set V[X] = V[X] - V[Y], V[F] = NOT borrow.
                    chip8->V[0xF] = (chip8->V[chip8->instruction.X] >= chip8->V[chip8->instruction.Y]) ? 1 : 0;
                    chip8->V[chip8->instruction.X] -= chip8->V[chip8->instruction.Y];
                    break;
                case 6: // 8XY6: SHR Vx {, Vy} - Set V[X] = V[X] SHR 1.
                    // Original CHIP-8: Vx = Vx >> 1. VF = LSB of Vx BEFORE shift.
                    // Some SCHIP variants/emulators: Vx = Vy >> 1. This emulates original.
                    // if (config.legacy_8XY6_shift_vy) { // Optional config
                    //    chip8->V[0xF] = chip8->V[chip8->instruction.Y] & 0x1;
                    //    chip8->V[chip8->instruction.X] = chip8->V[chip8->instruction.Y] >> 1;
                    // } else {
                        chip8->V[0xF] = chip8->V[chip8->instruction.X] & 0x1; // LSB to VF
                        chip8->V[chip8->instruction.X] >>= 1;
                    // }
                    break;
                case 7: // 8XY7: SUBN Vx, Vy - Set V[X] = V[Y] - V[X], V[F] = NOT borrow.
                    chip8->V[0xF] = (chip8->V[chip8->instruction.Y] >= chip8->V[chip8->instruction.X]) ? 1 : 0;
                    chip8->V[chip8->instruction.X] = chip8->V[chip8->instruction.Y] - chip8->V[chip8->instruction.X];
                    break;
                case 0xE: // 8XYE: SHL Vx {, Vy} - Set V[X] = V[X] SHL 1.
                    // Original CHIP-8: Vx = Vx << 1. VF = MSB of Vx BEFORE shift.
                    // Some SCHIP variants/emulators: Vx = Vy << 1. This emulates original.
                    // if (config.legacy_8XYE_shift_vy) { // Optional config
                    //    chip8->V[0xF] = (chip8->V[chip8->instruction.Y] & 0x80) >> 7;
                    //    chip8->V[chip8->instruction.X] = chip8->V[chip8->instruction.Y] << 1;
                    // } else {
                        chip8->V[0xF] = (chip8->V[chip8->instruction.X] & 0x80) >> 7; // MSB to VF
                        chip8->V[chip8->instruction.X] <<= 1;
                    // }
                    break;
                default:
                    #ifdef DEBUG
                    fprintf(stderr, "Warning: Unhandled 8XYN opcode 0x%04X (PC: 0x%04X)\n", chip8->instruction.opcode, chip8->PC-2);
                    #endif
                    break;
            }
            break;

        case 0x09: // 9XY0: SNE Vx, Vy - Skip next instruction if V[X] != V[Y]
            if (chip8->instruction.N == 0) { // Ensure last nibble is 0
                if (chip8->V[chip8->instruction.X] != chip8->V[chip8->instruction.Y]) {
                    chip8->PC += 2;
                }
            } else { // Invalid opcode (9XYN where N!=0)
                 #ifdef DEBUG
                fprintf(stderr, "Warning: Invalid 9XYN opcode 0x%04X (N!=0) (PC: 0x%04X)\n", chip8->instruction.opcode, chip8->PC-2);
                #endif
            }
            break;

        case 0x0A: // ANNN: LD I, addr - Set I = NNN
            chip8->I = chip8->instruction.NNN;
            break;

        case 0x0B: // BNNN: JP V0, addr - Jump to location NNN + V[0]
            chip8->PC = chip8->instruction.NNN + chip8->V[0];
            break;

        case 0x0C: // CXNN: RND Vx, byte - Set V[X] = random byte AND NN
            chip8->V[chip8->instruction.X] = (rand() % 256) & chip8->instruction.NN;
            break;

        case 0x0D: { // DXYN: DRW Vx, Vy, nibble - Display N-byte sprite starting at memory I at (VX, VY), set VF = collision.
            uint8_t x_coord = chip8->V[chip8->instruction.X];
            uint8_t y_coord = chip8->V[chip8->instruction.Y];
            uint8_t height = chip8->instruction.N;
            uint8_t sprite_byte;
            uint32_t screen_idx;

            chip8->V[0xF] = 0; // Reset collision flag

            for (uint8_t row = 0; row < height; row++) {
                // Ensure sprite data is read within RAM bounds
                if ((size_t)chip8->I + row >= sizeof(chip8->ram)) {
                    #ifdef DEBUG
                    fprintf(stderr, "Warning: Sprite draw (DXYN) attempting to read I (0x%04X + %u) out of RAM bounds (PC: 0x%04X).\n", chip8->I, row, chip8->PC-2);
                    #endif
                    break; // Stop if trying to read sprite data out of RAM
                }
                sprite_byte = chip8->ram[chip8->I + row];

                for (uint8_t col_bit = 0; col_bit < 8; col_bit++) {
                    // Calculate actual screen coordinates with wrapping
                    uint8_t current_x = (x_coord + col_bit) % config.window_width;
                    uint8_t current_y = (y_coord + row) % config.window_height;

                    if ((sprite_byte & (0x80 >> col_bit))) { // If current sprite pixel bit is 1
                        screen_idx = current_y * config.window_width + current_x;
                        
                        // This check should technically be redundant if width/height are correct
                        // but as a safeguard:
                        if (screen_idx < (config.window_width * config.window_height)) {
                            if (chip8->display[screen_idx]) { // Collision
                                chip8->V[0xF] = 1;
                            }
                            chip8->display[screen_idx] ^= 1; // XOR pixel
                        }
                    }
                }
            }
            break;
        }


        case 0x0E: // EXNN: Key-related operations
            switch (chip8->instruction.NN) {
                case 0x9E: // EX9E: SKP Vx - Skip next instruction if key with the value of V[X] is pressed
                    if (chip8->keypad[chip8->V[chip8->instruction.X] & 0xF]) { // Mask VX to 0-F
                        chip8->PC += 2;
                    }
                    break;
                case 0xA1: // EXA1: SKNP Vx - Skip next instruction if key with the value of V[X] is NOT pressed
                    if (!chip8->keypad[chip8->V[chip8->instruction.X] & 0xF]) { // Mask VX to 0-F
                        chip8->PC += 2;
                    }
                    break;
                default:
                     #ifdef DEBUG
                    fprintf(stderr, "Warning: Unhandled EXNN opcode 0x%04X (PC: 0x%04X)\n", chip8->instruction.opcode, chip8->PC-2);
                    #endif
                    break;
            }
            break;

        case 0x0F: // FXNN: Miscellaneous operations
            switch (chip8->instruction.NN) {
                case 0x07: // FX07: LD Vx, DT - Set V[X] = delay timer value
                    chip8->V[chip8->instruction.X] = chip8->delay_timer;
                    break;
                case 0x0A: { // FX0A: LD Vx, K - Wait for a key press, store the value of the key in V[X]
                    bool key_pressed_this_cycle = false;
                    for (uint8_t i = 0; i < sizeof(chip8->keypad)/sizeof(chip8->keypad[0]); i++) { // Iterate 0-15
                        if (chip8->keypad[i]) {
                            chip8->V[chip8->instruction.X] = i;
                            key_pressed_this_cycle = true;
                            break;
                        }
                    }
                    if (!key_pressed_this_cycle) {
                        chip8->PC -= 2; // Repeat instruction
                    }
                    break;
                }
                case 0x15: // FX15: LD DT, Vx - Set delay timer = V[X]
                    chip8->delay_timer = chip8->V[chip8->instruction.X];
                    break;
                case 0x18: // FX18: LD ST, Vx - Set sound timer = V[X]
                    chip8->sound_timer = chip8->V[chip8->instruction.X];
                    break;
                case 0x1E: // FX1E: ADD I, Vx - Set I = I + V[X]
                    // Undocumented: Some interpreters set VF on overflow (I > 0xFFF). Most modern ones don't.
                    // uint16_t new_I = chip8->I + chip8->V[chip8->instruction.X];
                    // if (new_I < chip8->I) chip8->V[0xF] = 1; else chip8->V[0xF] = 0; // Example if VF needed for I overflow
                    chip8->I += chip8->V[chip8->instruction.X];
                    break;
                case 0x29: // FX29: LD F, Vx - Set I = location of sprite for digit V[X]
                    chip8->I = (chip8->V[chip8->instruction.X] & 0xF) * 5; // Font sprites are 5 bytes each
                    break;
                case 0x33: { // FX33: LD B, Vx - Store BCD representation of V[X] in memory locations I, I+1, and I+2
                    uint8_t val = chip8->V[chip8->instruction.X];
                    if ((size_t)chip8->I + 2 < sizeof(chip8->ram)) {
                        chip8->ram[chip8->I]     = val / 100;        // Hundreds
                        chip8->ram[chip8->I + 1] = (val / 10) % 10;  // Tens
                        chip8->ram[chip8->I + 2] = val % 10;         // Ones
                    } else {
                        #ifdef DEBUG
                        fprintf(stderr, "Warning: BCD Store (FX33) attempting to write I (0x%04X) out of RAM bounds (PC: 0x%04X).\n", chip8->I, chip8->PC-2);
                        #endif
                    }
                    break;
                }
                case 0x55: { // FX55: LD [I], Vx - Store registers V0 through V[X] in memory starting at location I
                    // Ensure no buffer overflow.
                    if ((size_t)chip8->I + chip8->instruction.X < sizeof(chip8->ram)) {
                        for (uint8_t i = 0; i <= chip8->instruction.X; i++) {
                            chip8->ram[chip8->I + i] = chip8->V[i];
                        }
                        // Apply modern behavior: I = I + X + 1
                        // if (!config.legacy_fx55_fx65_I_behavior) { // If you add a config option
                            chip8->I += (chip8->instruction.X + 1);
                        // }
                    } else {
                         #ifdef DEBUG
                        fprintf(stderr, "Warning: Register Dump (FX55) attempting to write from I (0x%04X up to +%u) out of RAM bounds (PC: 0x%04X).\n", chip8->I, chip8->instruction.X, chip8->PC-2);
                        #endif
                    }
                    break;
                }
                case 0x65: { // FX65: LD Vx, [I] - Read registers V0 through V[X] from memory starting at location I
                    // Ensure no buffer overflow.
                    if ((size_t)chip8->I + chip8->instruction.X < sizeof(chip8->ram)) {
                        for (uint8_t i = 0; i <= chip8->instruction.X; i++) {
                            chip8->V[i] = chip8->ram[chip8->I + i];
                        }
                        // Apply modern behavior: I = I + X + 1
                        // if (!config.legacy_fx55_fx65_I_behavior) { // If you add a config option
                             chip8->I += (chip8->instruction.X + 1);
                        // }
                    } else {
                         #ifdef DEBUG
                        fprintf(stderr, "Warning: Register Load (FX65) attempting to read from I (0x%04X up to +%u) out of RAM bounds (PC: 0x%04X).\n", chip8->I, chip8->instruction.X, chip8->PC-2);
                        #endif
                    }
                    break;
                }
                default:
                     #ifdef DEBUG
                    fprintf(stderr, "Warning: Unhandled FXNN opcode 0x%04X (PC: 0x%04X)\n", chip8->instruction.opcode, chip8->PC-2);
                    #endif
                    break;
            }
            break;

        default:
            #ifdef DEBUG
            fprintf(stderr, "Warning: Unhandled high nibble for opcode 0x%04X (PC: 0x%04X)\n", chip8->instruction.opcode, chip8->PC-2);
            #endif
            break;
    }
    (void)config; // Suppress unused parameter warning if config is not used by emulate_instr directly
}