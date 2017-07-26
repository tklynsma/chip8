#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

class Chip8;

typedef unsigned char byte;
typedef unsigned short word;
typedef void (Chip8::*Operation)();

const int MEM_SIZE       = 4096;
const int REG_SIZE       = 16;
const int STACK_SIZE     = 16;
const int DISPLAY_WIDTH  = 64;
const int DISPLAY_HEIGHT = 32;
const int NUM_KEYS       = 16;

class Chip8 {
    public:
        Chip8();
        void initialize();
        void cycle();
        void update_timers();
        void load_rom(char* data, int num_bytes);
        void set_key(byte index, bool value);
        bool is_pixel(int x, int y);
        bool is_draw_flag();
        void reset_draw_flag();
    private:
        // Memory and general purpose registers:
        byte memory_[MEM_SIZE], V_[REG_SIZE];

        // The index register and program counter:
        word I_, pc_;

        // The stack and stack pointer:
        word stack_[STACK_SIZE];
        byte sp_;

        // Delay and sound timers:
        byte delay_timer_, sound_timer_;

        // Current opcode:
        word opcode_;

        // The display:
        bool display_[DISPLAY_WIDTH][DISPLAY_HEIGHT];
        bool draw_flag_;

        // Key status:
        bool key_[NUM_KEYS], store_key_;
        byte key_index_;

        // Decoding and executing operations:
        void exec_operation();  // Decode and execute the operation.
        void exec_zero();       // Decode and execute operations starting with 0.
        void exec_arithmetic(); // Decode and execute operations starting with 8.
        void exec_key();        // Decode and execute operations starting with E.
        void exec_memory();     // Decode and execute operations starting with F.

        // Instructions:
        void nop();
        void clear();           // 00E0: Clears the screen.
        void ret();             // 00EE: Return from subroutine.
        void jump();            // 1NNN: Jump to address NNN.
        void call();            // 2NNN: Calls subroutine at NNN.
        void skip_eq_const();   // 3XNN: Skips the next instruction if VX == NN.
        void skip_neq_const();  // 4XNN: Skips the next instruction if VX != NN.
        void skip_eq();         // 5XY0: Skips the next instruction if VX == VY.
        void assign_const();    // 6XNN: Set VX to NN.
        void add_const();       // 7XNN: Add NN to VX.
        void assign();          // 8XY0: Assign VY to VX.
        void bitwise_or();      // 8XY1: Set VX to VX or VY (Bitwise OR).
        void bitwise_and();     // 8XY2: Set VX to VX and VY (Bitwise AND).
        void bitwise_xor();     // 8XY3: Set VX to VX xor VY (Bitwise XOR).
        void add();             // 8XY4: Adds VY to VX.
        void sub();             // 8XY5: Sets VX to VX minus VY.
        void shift_right();     // 8XY6: Shift VX right by 1.
        void sub_reverse();     // 8XY7: Sets VX to VY minus VX.
        void shift_left();      // 8XYE: Shift VX left by 1.
        void skip_neq();        // 9XY0: Skips the next instruction if VX != VY.
        void set_index();       // ANNN: Sets I to the address NNN.
        void jump_offset();     // BNNN: Jumps to the address NNN plus V0.
        void random_number();   // CXNN: Sets VX to a random number with a mask of NN.
        void draw();            // DXYN: Draws the sprite (8xN) at addr I to (VX, VY).
        void skip_eq_key();     // EX9E: Skips the next instr if key VX is pressed.
        void skip_neq_key();    // EXA1: Skips the next instr if key VX isn't pressed.
        void get_delay();       // FX07: Sets VX to the value of the delay timer.
        void get_key();         // FX0A: A key press is awaited, and stored in VX.
        void set_delay();       // FX15: Sets the delay timer to VX.
        void set_sound();       // FX18: Sets the sound timer to VX.
        void add_index();       // FX1E: Adds VX to I.
        void sprite_addr();     // FX29: Sets I to the location of the sprite VX.
        void bcd();             // FX33: Stores the bcd repr of VX at addr I.
        void reg_dump();        // FX55: Stores V0 to VX in memory starting at addr I.
        void reg_load();        // FX65: Fills V0 to VX from mem starting at addr I.

    #if defined(UNIT_TEST)
    friend class Chip8Test;
    #endif
};

#endif //CHIP8_H
