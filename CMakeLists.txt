cmake_minimum_required(VERSION 2.8)

project(chip8)

set(SOURCE_FILES src/main.cpp src/chip8.cpp src/chip8.h)
add_executable(chip8_emulator ${SOURCE_FILES})

set(TEST_SOURCE_FILES test/catch.hpp test/test_chip8.cpp test/test_main.cpp src/chip8.cpp src/chip8.h)
add_executable(chip8_tests ${TEST_SOURCE_FILES})
