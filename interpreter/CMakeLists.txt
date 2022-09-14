target_sources(chip8 PRIVATE interpreter.c read.c)
target_include_directories(chip8 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
