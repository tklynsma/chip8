#define UNIT_TEST

#include "catch.hpp"
#include "../src/chip8.h"

// Help class for unit testing.

class Chip8Test {
    public:
        Chip8Test() { cpu_ = new Chip8(); }
        ~Chip8Test() { delete cpu_; }
        void initialize() { cpu_->initialize(); }
        void cycle() { cpu_->cycle(); }

        void set_index(word address) { cpu_->I_ = address; }
        void load_memory(word address, byte val) { cpu_->memory_[address] = val; }
        void load_register(byte index, byte val) { cpu_->V_[index] = val; }

        word get_pc() { return cpu_->pc_; }
        word get_index() { return cpu_->I_; }
        byte get_memory(word address) { return cpu_->memory_[address]; }
        byte get_register(byte index) { return cpu_->V_[index]; }
    private:
        Chip8* cpu_;
};

// Chip8 instruction tests:

TEST_CASE("op_1NNN", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x1A);
    cpu.load_memory(0x201, 0xBC);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0xABC );
}

TEST_CASE("op_3XNN", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x3A);
    cpu.load_memory(0x201, 0x43);
    cpu.load_register(0xA, 0x44);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_memory(0x202, 0x3A);
    cpu.load_memory(0x203, 0x44);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_4XNN", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x4A);
    cpu.load_memory(0x201, 0x33);
    cpu.load_register(0xA, 0x33);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_memory(0x202, 0x4A);
    cpu.load_memory(0x203, 0x34);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_5XY0", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x5A);
    cpu.load_memory(0x201, 0xB0);
    cpu.load_register(0xA, 0x11);
    cpu.load_register(0xB, 0x12);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_memory(0x202, 0x5A);
    cpu.load_memory(0x203, 0xB0);
    cpu.load_register(0xB, 0x11);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_6XNN", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x6A);
    cpu.load_memory(0x201, 0xBC);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xBC );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_7XNN", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x7A);
    cpu.load_memory(0x201, 0x11);
    cpu.load_register(0xA, 0x11);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x22 );
    REQUIRE( cpu.get_pc() == 0x202 );
    
    cpu.load_memory(0x202, 0x7A);
    cpu.load_memory(0x203, 0xDE);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XY0", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xB0);
    cpu.load_register(0xB, 0xAB);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xAB );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_8XY1", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xB1);
    cpu.load_register(0xA, 0x91);
    cpu.load_register(0xB, 0x19);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x99 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_8XY2", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xB2);
    cpu.load_register(0xA, 0x91);
    cpu.load_register(0xB, 0x19);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x11 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_8XY3", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xB3);
    cpu.load_register(0xA, 0x91);
    cpu.load_register(0xB, 0x19);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x88 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_8XY4", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xB4);
    cpu.load_register(0xA, 0x11);
    cpu.load_register(0xB, 0x11);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x22 );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_memory(0x202, 0x8A);
    cpu.load_memory(0x203, 0xB4);
    cpu.load_register(0xB, 0xDE);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x00 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XY5", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xB5);
    cpu.load_register(0xA, 0x22);
    cpu.load_register(0xB, 0x11);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x11 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_memory(0x202, 0x8A);
    cpu.load_memory(0x203, 0xB5);
    cpu.load_register(0xB, 0x12);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xFF );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XY6", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xB6);
    cpu.load_register(0xA, 0x06);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x03 );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_memory(0x202, 0x8A);
    cpu.load_memory(0x203, 0xB6);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x01 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XY7", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xB7);
    cpu.load_register(0xA, 0x11);
    cpu.load_register(0xB, 0x22);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x11 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_memory(0x202, 0x8A);
    cpu.load_memory(0x203, 0xB7);
    cpu.load_register(0xB, 0x10);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xFF );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XYE", "[cpu]") {
    Chip8Test cpu;
    cpu.initialize();
    cpu.load_memory(0x200, 0x8A);
    cpu.load_memory(0x201, 0xBE);
    cpu.load_register(0xA, 0x70);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xE0 );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_memory(0x202, 0x8A);
    cpu.load_memory(0x203, 0xBE);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xC0 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x204 );
}
