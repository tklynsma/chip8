#include "chip8.h"

Chip8::Chip8() { srand(time(NULL)); }

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
    for (int i = 0; i < MEM_SIZE; i++) {
        memory_[i] = 0;
    }

    // Clear general purpose registers:
    for (int i = 0; i < REG_SIZE; i++) {
        V_[i] = 0;
    }

    // Clear the stack:
    for (int i = 0; i < STACK_SIZE; i++) {
        stack_[i] = 0;
    }

    // Clear display
    draw_flag_ = true;
    for (int i = 0; i < DISPLAY_WIDTH; i++) {
        for (int j = 0; j < DISPLAY_HEIGHT; j++) {
            display_[i][j] = 0;
        }
    }

    // Reset keys
    for (int i = 0; i < NUM_KEYS; i++) {
        key_[i] = false;
    }

    // Load fontset:
    for (int i = 0; i < 80; i++) {
        memory_[i] = fontset[i];
    }
    
    // Reset timers:
    delay_timer_ = sound_timer_ = 0;

    // Reset store key flag:
    store_key_ = false;
}

// Fetch, decode and execute the next operation.
void Chip8::cycle() {
    opcode_ = memory_[pc_] << 8 | memory_[pc_ + 1];
    exec_operation();
}

// Load the rom into memory.
void Chip8::load_rom(char* data, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        memory_[0x200 + i] = (byte) data[i];
    }
}

// Update the delay and sound timer.
void Chip8::update_timers() {
    if (delay_timer_ > 0) { delay_timer_--; }
    if (sound_timer_ > 0) { sound_timer_--; }
}

void Chip8::set_key(byte index, bool value) {
    key_[index] = value;

    if (store_key_ && value) {
        V_[key_index_] = index;
        store_key_ = false;
        pc_ += 2;
    }
}

bool Chip8::is_pixel(int x, int y) { return display_[x][y]; }
bool Chip8::is_draw_flag() { return draw_flag_; }
bool Chip8::is_sound_flag() { return sound_flag_; }
void Chip8::reset_draw_flag() { draw_flag_ = false; }
void Chip8::reset_sound_flag() { sound_flag_ = false; }
byte Chip8::get_sound_duration() { return sound_duration_; }

// Decode and execute the operation.
void Chip8::exec_operation() {
    switch ((opcode_ & 0xF000) >> 12) {
        case 0x0:   exec_zero();        break;  // 0XYZ
        case 0x1:   jump();             break;  // 1NNN
        case 0x2:   call();             break;  // 2NNN
        case 0x3:   skip_eq_const();    break;  // 3XNN
        case 0x4:   skip_neq_const();   break;  // 4XNN
        case 0x5:   skip_eq();          break;  // 5XY0
        case 0x6:   assign_const();     break;  // 6XNN
        case 0x7:   add_const();        break;  // 7XNN
        case 0x8:   exec_arithmetic();  break;  // 8XYZ
        case 0x9:   skip_neq();         break;  // 9XY0
        case 0xA:   set_index();        break;  // ANNN
        case 0xB:   jump_offset();      break;  // BNNN
        case 0xC:   random_number();    break;  // CXNN
        case 0xD:   draw();             break;  // DXYN
        case 0xE:   exec_key();         break;  // EXYZ
        case 0xF:   exec_memory();      break;  // FXYZ
        default:    nop();              break;
    }
}

// Decode and execute operations starting with 0.
void Chip8::exec_zero() {
    switch (opcode_ & 0x0FFF) {
        case 0x0E0: clear();            break;  // 00E0
        case 0x0EE: ret();              break;  // 00EE
        default:    nop();              break;
    }
}

// Decode and execute operations starting with 8.
void Chip8::exec_arithmetic() {
    switch (opcode_ & 0x000F) {
        case 0x0:   assign();           break;  // 8XY0
        case 0x1:   bitwise_or();       break;  // 8XY1
        case 0x2:   bitwise_and();      break;  // 8XY2
        case 0x3:   bitwise_xor();      break;  // 8XY3
        case 0x4:   add();              break;  // 8XY4
        case 0x5:   sub();              break;  // 8XY5
        case 0x6:   shift_right();      break;  // 8XY6
        case 0x7:   sub_reverse();      break;  // 8XY7
        case 0xE:   shift_left();       break;  // 8XYE
        default:    nop();              break;
    }
}

// Decode and execute operations starting with E.
void Chip8::exec_key() {
    switch (opcode_ & 0x00FF) {
        case 0x9E:  skip_eq_key();      break;  // EX9E
        case 0xA1:  skip_neq_key();     break;  // EXA1
        default:    nop();              break;
    }
}

// Decode and execute operations starting with F.
void Chip8::exec_memory() {
    switch (opcode_ & 0x00FF) {
        case 0x07:  get_delay();        break;  // FX07
        case 0x0A:  get_key();          break;  // FX0A
        case 0x15:  set_delay();        break;  // FX15
        case 0x18:  set_sound();        break;  // FX18
        case 0x1E:  add_index();        break;  // FX1E
        case 0x29:  sprite_addr();      break;  // FX29
        case 0x33:  bcd();              break;  // FX33
        case 0x55:  reg_dump();         break;  // FX55
        case 0x65:  reg_load();         break;  // FX65
        default:    nop();              break;
    }
}

// Invalid operation: do nothing.
void Chip8::nop() {
    pc_ += 2;
}

// 00E0: Clears the screen.
void Chip8::clear() {
    for (int i = 0; i < DISPLAY_WIDTH; i++) {
        for (int j = 0; j < DISPLAY_HEIGHT; j++) {
            display_[i][j] = 0;
        }
    }
    draw_flag_ = true;
    pc_ += 2;
}

// 00EE: Return from subroutine.
void Chip8::ret() {
    pc_ = stack_[--sp_] + 2;
}

// 1NNN: Jump to address NNN.
void Chip8::jump() {
    pc_ = opcode_ & 0x0FFF;
}

// 2NNN: Calls subroutine at NNN.
void Chip8::call() {
    stack_[sp_++] = pc_;
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
    V_[X] = V_[X] + V_[Y];
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

// CXNN: Sets VX to a random number with a mask of NN.
void Chip8::random_number() {
    V_[(opcode_ & 0x0F00) >> 8] = (rand() % 256) & (opcode_ & 0x00FF);
    pc_ += 2;
}

// DXYN: Draws the (bit-coded) sprite stored at address I at coordinate (VX, VY)
// of size 8xN. Set VF to 1 if any pixel is flipped from set to unset (collision).
void Chip8::draw() {
    int x_start = V_[(opcode_ & 0x0F00) >> 8];
    int y_start = V_[(opcode_ & 0x00F0) >> 4];
    int height  = opcode_ & 0x000F;
    V_[0xF] = 0x00;

    for (int line = 0; line < height; line++) {
        for (int bit = 0; bit < 8; bit++) {
            bool pixel = (memory_[I_ + line] >> (7 - bit)) & 0x01;
            int x = (x_start + bit ) % DISPLAY_WIDTH;
            int y = (y_start + line) % DISPLAY_HEIGHT;
            if (display_[x][y] && pixel) {
                V_[0xF] = 0x01; // collision
            }
            display_[x][y] ^= pixel;
        }
    }

    draw_flag_ = true;
    pc_ += 2;
}

// EX9E: Skips the next instruction if the key stored in VX is pressed.
void Chip8::skip_eq_key() {
    pc_ = key_[V_[(opcode_ & 0x0F00) >> 8]] ? pc_ + 4 : pc_ + 2;
}

// EXA1: Skips the next instruction if the key stored in VX is not pressed.
void Chip8::skip_neq_key() {
    pc_ = key_[V_[(opcode_ & 0x0F00) >> 8]] ? pc_ + 2 : pc_ + 4;
}

// FX07: Sets VX to the value of the delay timer.
void Chip8::get_delay() {
    V_[(opcode_ & 0x0F00) >> 8] = delay_timer_;
    pc_ += 2;
}

// FX0A: A key press is awaited, and then stored in VX.
void Chip8::get_key() {
    if (!store_key_) {
        // Check if any key is already pressed and if so store it in VX.
        for (byte i = 0; i < 16; i++) {
            if (key_[i]) {
                V_[(opcode_ & 0x0F00) >> 8] = i;
                pc_ += 2;
                return;
            }
        }

        // If no key was pressed then store the next keypress in VX.
        key_index_ = (opcode_ & 0x0F00) >> 8;
        store_key_ = true;
    }
}

// FX15: Sets the delay timer to VX.
void Chip8::set_delay() {
    delay_timer_ = V_[(opcode_ & 0x0F00) >> 8];
    pc_ += 2;
}

// FX18: Sets the sound timer to VX.
void Chip8::set_sound() {
    if (!sound_flag_) {
        sound_timer_ = sound_duration_ = V_[(opcode_ & 0x0F00) >> 8];
        sound_flag_ = true;
        pc_ += 2;
    }
}

// FX1E: Adds VX to I.
void Chip8::add_index() {
    I_ += V_[(opcode_ & 0x0F00) >> 8];
    pc_ += 2;
}

// FX29: Sets I to the location of the sprite for the character in VX.
void Chip8::sprite_addr() {
    I_ = 5 * V_[(opcode_ & 0x0F00) >> 8];
    pc_ += 2;
}

// FX33: Stores the binary-coded-decimal representation of VX, with the most signi-
// ficant of three digits at address I, the middle digit at addres I plus one, and
// and the least significant digit at I plus two.
void Chip8::bcd() {
    byte X = (opcode_ & 0x0F00) >> 8;
    memory_[I_]   = V_[X] / 100;
    memory_[I_+1] = (V_[X] / 10) % 10;
    memory_[I_+2] = (V_[X] % 100) % 10;
    pc_ += 2;
}

// FX55: Stores V0 to VX (including) in memory starting at address I.
void Chip8::reg_dump() {
    for (byte i = 0; i <= (opcode_ & 0x0F00) >> 8; i++) {
        memory_[I_+i] = V_[i];
    }
    pc_ += 2;
}

// FX65: Fills V0 to VX (including) with values from memory starting at address I.
void Chip8::reg_load() {
    for (byte i = 0; i <= (opcode_ & 0x0F00) >> 8; i++) {
        V_[i] = memory_[I_+i];
    }
    pc_ += 2;
}
