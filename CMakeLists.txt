cmake_minimum_required(VERSION 2.8)

project(chip8)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SOURCE_FILES src/main.cpp src/chip8.cpp src/chip8.h)
add_executable(chip8_emulator ${SOURCE_FILES})
target_link_libraries(chip8_emulator ${SDL2_LIBRARIES})

set(TEST_SOURCE_FILES test/catch.hpp test/test_chip8.cpp test/test_main.cpp test/util.h src/chip8.cpp src/chip8.h)
add_executable(chip8_tests ${TEST_SOURCE_FILES})
