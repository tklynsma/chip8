#define UNIT_TEST

#include "catch.hpp"
#include "util.h"

TEST_CASE("op_00EE_and_2NNN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x2ABC);
    cpu.cycle();
    REQUIRE( cpu.get_sp() == 1 );
    REQUIRE( cpu.get_pc() == 0xABC );
    REQUIRE( cpu.get_stack(cpu.get_sp() - 1) == 0x200 );

    cpu.load_opcode(0xABC, 0x00EE);
    cpu.cycle();
    REQUIRE( cpu.get_sp() == 0 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_1NNN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x1ABC);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0xABC );
}

TEST_CASE("op_3XNN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x3A43);
    cpu.load_register(0xA, 0x44);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x3A44);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_4XNN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x4A33);
    cpu.load_register(0xA, 0x33);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x4A34);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_5XY0", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x5AB0);
    cpu.load_register(0xA, 0x11);
    cpu.load_register(0xB, 0x12);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x5AB0);
    cpu.load_register(0xB, 0x11);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_6XNN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x6ABC);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xBC );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_7XNN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x7A11);
    cpu.load_register(0xA, 0x11);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x22 );
    REQUIRE( cpu.get_pc() == 0x202 );
    
    cpu.load_opcode(0x202, 0x7ADE);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XY0", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8AB0);
    cpu.load_register(0xB, 0xAB);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xAB );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_8XY1", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8AB1);
    cpu.load_register(0xA, 0x91);
    cpu.load_register(0xB, 0x19);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x99 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_8XY2", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8AB2);
    cpu.load_register(0xA, 0x91);
    cpu.load_register(0xB, 0x19);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x11 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_8XY3", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8AB3);
    cpu.load_register(0xA, 0x91);
    cpu.load_register(0xB, 0x19);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x88 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_8XY4", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8AB4);
    cpu.load_register(0xA, 0x11);
    cpu.load_register(0xB, 0x11);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x22 );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x8AB4);
    cpu.load_register(0xB, 0xDE);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x00 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XY5", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8AB5);
    cpu.load_register(0xA, 0x22);
    cpu.load_register(0xB, 0x11);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x11 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x8AB5);
    cpu.load_register(0xB, 0x12);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xFF );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XY6", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8AB6);
    cpu.load_register(0xA, 0x06);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x03 );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x8AB6);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x01 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XY7", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8AB7);
    cpu.load_register(0xA, 0x11);
    cpu.load_register(0xB, 0x22);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x11 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x8AB7);
    cpu.load_register(0xB, 0x10);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xFF );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_8XYE", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x8ABE);
    cpu.load_register(0xA, 0x70);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xE0 );
    REQUIRE( cpu.get_register(0xF) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x8ABE);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0xC0 );
    REQUIRE( cpu.get_register(0xF) == 0x01 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_9XY0", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0x9AB0);
    cpu.load_register(0xA, 0x44);
    cpu.load_register(0xB, 0x44);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0x9AB0);
    cpu.load_register(0xA, 0x43);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_ANNN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xAABC);
    cpu.cycle();
    REQUIRE( cpu.get_index() == 0xABC );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_BNNN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xBAB0);
    cpu.load_register(0x0, 0x0C);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0xABC );
}

TEST_CASE("op_00E0_and_DXYN", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xDAB2);
    cpu.load_register(0xA, 0x01);
    cpu.load_register(0xB, 0x04);
    cpu.load_register(0xF, 0x01);
    cpu.load_memory(0xA00, 0x80);
    cpu.load_memory(0xA01, 0x40);
    cpu.set_index(0xA00);
    cpu.cycle();
    REQUIRE( cpu.get_display(1, 4) );
    REQUIRE(!cpu.get_display(2, 4) );
    REQUIRE(!cpu.get_display(1, 5) );
    REQUIRE( cpu.get_display(2, 5) );
    REQUIRE( cpu.get_register(0xF) == 0 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0xDAB1);
    cpu.load_memory(0xA00, 0xC0);
    cpu.cycle();
    REQUIRE(!cpu.get_display(1, 4) );
    REQUIRE( cpu.get_display(2, 4) );
    REQUIRE( cpu.get_register(0xF) == 1 );
    REQUIRE( cpu.get_pc() == 0x204 );

    cpu.load_opcode(0x204, 0x00E0);
    cpu.cycle();
    REQUIRE(!cpu.get_display(1, 4) );
    REQUIRE(!cpu.get_display(2, 4) );
    REQUIRE(!cpu.get_display(1, 5) );
    REQUIRE(!cpu.get_display(2, 5) );
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_EX9E", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xEA9E);
    cpu.load_register(0xA, 0x02);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.load_opcode(0x202, 0xEA9E);
    cpu.set_key(0x02, true);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_EXA1", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xEAA1);
    cpu.load_register(0xA, 0x02);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x204 );

    cpu.load_opcode(0x204, 0xEAA1);
    cpu.set_key(0x02, true);
    cpu.cycle();
    REQUIRE( cpu.get_pc() == 0x206 );
}

TEST_CASE("op_FX07", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xFA07);
    cpu.set_delay_timer(0x33);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x33 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_FX0A", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xFA0A);
    cpu.set_key(0x07, true);
    cpu.set_key(0x03, true);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x03 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.set_key(0x03, false);
    cpu.set_key(0x07, false);
    cpu.load_register(0xA, 0x00);
    cpu.load_opcode(0x202, 0xFA0A);
    cpu.cycle();
    REQUIRE( cpu.get_register(0xA) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );

    cpu.set_key(0x03, true);
    cpu.set_key(0x07, true);
    REQUIRE( cpu.get_register(0xA) == 0x03 );
    REQUIRE( cpu.get_pc() == 0x204 );
}

TEST_CASE("op_FX15", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xFA15);
    cpu.load_register(0xA, 0x44);
    cpu.cycle();
    REQUIRE( cpu.get_delay_timer() == 0x44 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_FX18", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xFA18);
    cpu.load_register(0xA, 0x44);
    cpu.cycle();
    REQUIRE( cpu.get_sound_timer() == 0x44 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_FX1E", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xFA1E);
    cpu.load_register(0xA, 0x0C);
    cpu.set_index(0xAB0);
    cpu.cycle();
    REQUIRE( cpu.get_index() == 0xABC );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_FX29", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xFA29);
    cpu.load_register(0xA, 0x0A);
    cpu.cycle();
    REQUIRE( cpu.get_index() == 50 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_FX33", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xFA33);
    cpu.load_register(0xA, 0x7B);
    cpu.set_index(0x100);
    cpu.cycle();
    REQUIRE( cpu.get_memory(0x100) == 0x01 );
    REQUIRE( cpu.get_memory(0x101) == 0x02 );
    REQUIRE( cpu.get_memory(0x102) == 0x03 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_FX55", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xF155);
    cpu.load_register(0x0, 0x01);
    cpu.load_register(0x1, 0x02);
    cpu.load_register(0x2, 0x03);
    cpu.set_index(0x100);
    cpu.cycle();
    REQUIRE( cpu.get_memory(0x100) == 0x01 );
    REQUIRE( cpu.get_memory(0x101) == 0x02 );
    REQUIRE( cpu.get_memory(0x102) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );
}

TEST_CASE("op_FX65", "[cpu]") {
    Chip8Test cpu;
    cpu.load_opcode(0x200, 0xF165);
    cpu.load_memory(0x100, 0x01);
    cpu.load_memory(0x101, 0x02);
    cpu.load_memory(0x102, 0x03);
    cpu.set_index(0x100);
    cpu.cycle();
    REQUIRE( cpu.get_register(0x0) == 0x01 );
    REQUIRE( cpu.get_register(0x1) == 0x02 );
    REQUIRE( cpu.get_register(0x2) == 0x00 );
    REQUIRE( cpu.get_pc() == 0x202 );
}
