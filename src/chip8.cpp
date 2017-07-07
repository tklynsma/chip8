#include "chip8.h"

void Chip8::initialize() {
    static byte fontset[80] = {
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

    pc_ = 0x200; // Reset program counter.
    I_  = 0;     // Reset index register.
    sp_ = 0;     // Reset stack pointer.
    
    // Clear memory:
    for (int i = 0; i < 4096; i++) {
        memory_[i] = 0;
    }

    // Clear general purpose registers & stack:
    for (int i = 0; i < 16; i++) {
        V_[i] = stack_[i] = 0;
    }

    // Clear display
    // ...

    // Load fontset:
    for (int i = 0; i < 80; i++) {
        memory_[i] = fontset[i];
    }
    
    // Reset timers:
    delay_timer_ = sound_timer_ = 0;
}

// Fetch, decode and execute the next operation.
void Chip8::cycle() {
    opcode_ = memory_[pc_] << 8 | memory_[pc_ + 1];
    exec_operation();
}

// -----------------------------------------------------------------------------------
// Decoding and executing operations:
// -----------------------------------------------------------------------------------

// Decode and execute the operation.
void Chip8::exec_operation() {
    static Operation opcode_table[16] = {
        &Chip8::exec_zero,
        &Chip8::jump,               // 1NNN
        &Chip8::call,               // 2NNN
        &Chip8::skip_eq_const,      // 3XNN
        &Chip8::skip_neq_const,     // 4XNN
        &Chip8::skip_eq,            // 5XY0
        &Chip8::assign_const,       // 6XNN
        &Chip8::add_const,          // 7XNN
        &Chip8::exec_arithmetic,    // 8XYK
        &Chip8::skip_neq,           // 9XY0
        &Chip8::set_index,          // ANNN
        &Chip8::jump_offset,        // BNNN
        &Chip8::rand,               // CXNN
        &Chip8::draw,               // DXYN
        &Chip8::nop,
        &Chip8::nop
    };
    (this->*opcode_table[(opcode_ & 0xF000) >> 12])();
}

void Chip8::exec_zero() {
    //...
}

// Decode and execute the arithmetic operation 8XYK for some K.
void Chip8::exec_arithmetic() {
    static Operation opcode_table_arithmetic[16] = {
        &Chip8::assign,             // 8XY0
        &Chip8::bitwise_or,         // 8XY1
        &Chip8::bitwise_and,        // 8XY2
        &Chip8::bitwise_xor,        // 8XY3
        &Chip8::add,                // 8XY4
        &Chip8::sub,                // 8XY5
        &Chip8::shift_right,        // 8XY6
        &Chip8::sub_reverse,        // 8XY7
        &Chip8::nop,
        &Chip8::nop,
        &Chip8::nop,
        &Chip8::nop,
        &Chip8::nop,
        &Chip8::nop,
        &Chip8::shift_left,         // 8XYE
        &Chip8::nop,
    };
    (this->*opcode_table_arithmetic[opcode_ & 0x000F])();
}

void Chip8::exec_key() {
    //...
}

void Chip8::exec_memory() {
    //...
}

// -----------------------------------------------------------------------------------
// Chip8 instructions:
// -----------------------------------------------------------------------------------

// Invalid operation: do nothing.
void Chip8::nop() {
    pc_ += 2;
}

// 00EE: Return from subroutine.
void Chip8::ret() {
    pc_ = stack_[sp_--];
}

// 1NNN: Jump to address NNN.
void Chip8::jump() {
    pc_ = opcode_ & 0x0FFF;
}

// 2NNN: Calls subroutine at NNN.
void Chip8::call() {
    stack_[++sp_] = pc_;
    pc_ = opcode_ & 0x0FFF;
}

// 3XNN: Skips the next instruction if VX == NN.
void Chip8::skip_eq_const() {
    pc_ = V_[(opcode_ & 0x0F00) >> 8] == (opcode_ & 0x00FF) ? pc_ + 4 : pc_ + 2;
}

// 4XNN: Skips the next instruction if VX != NN.
void Chip8::skip_neq_const() {
    pc_ = V_[(opcode_ & 0x0F00) >> 8] != (opcode_ & 0x00FF) ? pc_ + 4 : pc_ + 2;
}

// 5XY0: Skips the next instruction if VX == VY.
void Chip8::skip_eq() {
    byte X = (opcode_ & 0x0F00) >> 8;
    byte Y = (opcode_ & 0x00F0) >> 4;
    pc_ = V_[X] == V_[Y] && (opcode_ & 0x000F) == 0 ? pc_ + 4 : pc_ + 2;
}

// 6XNN: Set VX to NN.
void Chip8::assign_const() {
    V_[(opcode_ & 0x0F00) >> 8]  = opcode_ & 0x00FF;
    pc_ += 2;
}

// 7XNN: Add NN to VX.
void Chip8::add_const() {
    V_[(opcode_ & 0x0F00) >> 8] += opcode_ & 0x00FF;
    pc_ += 2;
}

// 8XY0: Assign VY to VX.
void Chip8::assign() {
    V_[(opcode_ & 0x0F00) >> 8]  = V_[(opcode_ & 0x00F0) >> 4];
    pc_ += 2;
}

// 8XY1: Set VX to VX or VY (Bitwise OR).
void Chip8::bitwise_or() {
    V_[(opcode_ & 0x0F00) >> 8] |= V_[(opcode_ & 0x00F0) >> 4];
    pc_ += 2;
}

// 8XY2: Set VX to VX and VY (Bitwise AND).
void Chip8::bitwise_and() {
    V_[(opcode_ & 0x0F00) >> 8] &= V_[(opcode_ & 0x00F0) >> 4];
    pc_ += 2;
}

// 8XY3: Set VX to VX xor VY (Bitwise XOR).
void Chip8::bitwise_xor() {
    V_[(opcode_ & 0x0F00) >> 8] ^= V_[(opcode_ & 0x00F0) >> 4];
    pc_ += 2;
}

// 8XY4: Adds VY to VX. Set VF to 1 if there is a carry.
void Chip8::add() {
    byte X = (opcode_ & 0x0F00) >> 8;
    byte Y = (opcode_ & 0x00F0) >> 4;
    V_[0xF] = V_[X] + V_[Y] > 0xFF ? 1 : 0;
    V_[X] += V_[Y];
    pc_ += 2;
}

// 8XY5: Substract VY from VX. Set VF to 0 if there is a borrow.
void Chip8::sub() {
    byte X = (opcode_ & 0x0F00) >> 8;
    byte Y = (opcode_ & 0x00F0) >> 4;
    V_[0xF] = V_[X] < V_[Y] ? 0 : 1;
    V_[X] = V_[X] - V_[Y];
    pc_ += 2;
}

// 8XY6: Set VF to the least significant bit of VX and shift VX right by 1.
void Chip8::shift_right() {
    byte X = (opcode_ & 0x0F00) >> 8;
    V_[0xF] = V_[X] & 0x1;
    V_[X] >>= 1;
    pc_ += 2;
}

// 8XY7: Sets VX to VY minus VX. Set VF to 0 if there is a borrow.
void Chip8::sub_reverse() {
    byte X = (opcode_ & 0x0F00) >> 8;
    byte Y = (opcode_ & 0x00F0) >> 4;
    V_[0xF] = V_[Y] < V_[X] ? 0 : 1;
    V_[X] = V_[Y] - V_[X];
    pc_ += 2;
}

// 8XYE: Set VF to the most significant bit of VX and shift VX left by 1.
void Chip8::shift_left() {
    byte X = (opcode_ & 0x0F00) >> 8;
    V_[0xF] = V_[X] >> 7;
    V_[X] <<= 1;
    pc_ += 2;
}

// 9XY0: Skips the next instruction if VX != VY.
void Chip8::skip_neq() {
    byte X = (opcode_ & 0x0F00) >> 8;
    byte Y = (opcode_ & 0x00F0) >> 4;
    pc_ = V_[X] != V_[Y] && (opcode_ & 0x000F) == 0 ? pc_ + 4 : pc_ + 2;
}

// ANNN: Sets I to the address NNN.
void Chip8::set_index() {
    I_ = opcode_ & 0x0FFF;
    pc_ += 2;
}

// BNNN: Jumps to the address NNN plus V0.
void Chip8::jump_offset() {
    pc_ = V_[0] + (opcode_ & 0x0FFF);
}

// CXNN: Sets VX to the result of a bitwise AND on a random number and NN.
void Chip8::rand() {
    //V[(opcode_ & 0x0F00) >> 8] = ... & (opcode_ & 0x00FF);
}

// DXYN: Draws the (bit-coded) sprite stored at address I at coordinate (VX, VY)
// of size 8xN. Set VF to 1 if any pixel is flipped from set to unset (collision).
void Chip8::draw() {
    //...
}

// EX9E

// FX55: Stores V0 to VX (including) in memory starting at address I.
void Chip8::reg_dump() {
    for (byte i = 0; i <= (opcode_ & 0x0F00) >> 8; i++) {
        memory_[I_ + i] = V_[i];
    }
    pc_ += 2;
}

// FX65: Fills V0 to VX (including) with values from memory starting at address I.
void Chip8::reg_load() {
    for (byte i = 0; i <= (opcode_ & 0x0F00) >> 8; i++) {
        V_[i] = memory_[I_ + i];
    }
    pc_ += 2;
}
