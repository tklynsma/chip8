#ifndef CHIP8TEST_H
#define CHIP8TEST_H

#include "../src/chip8.h"

class Chip8Test {
    public:
        Chip8Test();
        ~Chip8Test();
        void initialize();
        void cycle();
        void set_index(word address);
        void set_delay_timer(byte value);
        void set_key(byte index, bool value);
        void load_register(byte index, byte val);
        void load_memory(word address, byte val);
        void load_opcode(word address, word opcode);
        word get_pc();
        byte get_sp();
        word get_index();
        byte get_delay_timer();
        byte get_sound_timer(); 
        word get_stack(byte index);
        byte get_memory(word address);
        byte get_register(byte index);
        bool get_display(int x, int y);
    private:
        Chip8* cpu_;
};

Chip8Test::Chip8Test()  { cpu_ = new Chip8(); cpu_->initialize(); }
Chip8Test::~Chip8Test() { delete cpu_;   }
void Chip8Test::cycle() { cpu_->cycle(); }

void Chip8Test::set_index(word address)             { cpu_->I_ = address;           }
void Chip8Test::set_delay_timer(byte value)         { cpu_->delay_timer_ = value;   }
void Chip8Test::set_key(byte index, bool value)     { cpu_->set_key(index, value);  }
void Chip8Test::load_register(byte index, byte val) { cpu_->V_[index] = val;        }
void Chip8Test::load_memory(word address, byte val) { cpu_->memory_[address] = val; }

void Chip8Test::load_opcode(word address, word opcode) {
    cpu_->memory_[address] = (opcode & 0xFF00) >> 8;
    cpu_->memory_[address + 1] = opcode & 0x00FF;
}

word Chip8Test::get_pc()                  { return cpu_->pc_;              }
byte Chip8Test::get_sp()                  { return cpu_->sp_;              }
word Chip8Test::get_index()               { return cpu_->I_;               }
byte Chip8Test::get_delay_timer()         { return cpu_->delay_timer_;     }
byte Chip8Test::get_sound_timer()         { return cpu_->sound_timer_;     }
word Chip8Test::get_stack(byte index)     { return cpu_->stack_[index];    }
byte Chip8Test::get_memory(word address)  { return cpu_->memory_[address]; }
byte Chip8Test::get_register(byte index)  { return cpu_->V_[index];        }
bool Chip8Test::get_display(int x, int y) { return cpu_->display_[x][y];   }

#endif //CHIP8TEST_H
