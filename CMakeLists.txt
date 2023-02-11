cmake_minimum_required(VERSION 3.16)

project(CHIP-8 VERSION 0.1)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8) 
target_sources(chip8 PRIVATE main.c)

add_subdirectory(utils)
add_subdirectory(interpreter)
add_subdirectory(graphics)

target_link_libraries(chip8 ${SDL2_LIBRARIES})

install(TARGETS chip8)
