#ifndef CHIP8_H
#define CHIP8_H

class Chip8;

typedef unsigned char byte;
typedef unsigned short word;
typedef void (Chip8::*Operation)();

class Chip8 {
    public:
        Chip8() {}
        void initialize();
        void cycle();
    private:
        // Memory and general purpose registers:
        byte memory_[4096], V_[16];

        // The index register and program counter:
        word I_, pc_;

        // The stack and stack pointer:
        word stack_[16];
        byte sp_;

        // Delay and sound timers:
        byte delay_timer_, sound_timer_;

        // Current opcode:
        word opcode_;

        // The display:
        bool display[64][32];

        // ---------------------------------------------------------------------------
        // Decoding and executing operations:
        // ---------------------------------------------------------------------------

        void exec_operation();  // Decode and execute the operation.
        void exec_zero();       // Decode and execute operations starting with 0.
        void exec_arithmetic(); // Decode and execute operations starting with 8.
        void exec_key();        // Decode and execute operations starting with E.
        void exec_memory();     // Decode and execute operations starting with F.

        // ---------------------------------------------------------------------------
        // Instructions:
        // ---------------------------------------------------------------------------
        
        void nop();
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
        void rand();            // CXNN: Sets VX to NN and a random number.
        void draw();            // DXYN: Draws the sprite (8xN) stored at address I to (VX, VY).
        void reg_dump();        // FX55: Stores V0 to VX in memory starting at address I.
        void reg_load();        // FX65: Fills V0 to VX from memory starting at address I.

    #if defined(UNIT_TEST)
    friend class Chip8Test;
    #endif
};

#endif //CHIP8_H
