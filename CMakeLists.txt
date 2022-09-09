cmake_minimum_required(VERSION 3.16)

project(CHIP-8 VERSION 0.1)

add_executable(chip8) 

target_sources(chip8 PRIVATE main.c)

add_subdirectory(utils)
add_subdirectory(interpreter)

install(TARGETS chip8)
