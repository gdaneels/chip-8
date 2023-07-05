## TODO

- [DONE] Test adding a instruction to memory and reading it out in the run() function
- [DONE] Add SDL library support to CMakeLists 
- [DONE] Create macro's to fetch first, second, third and forth nibbles of instruction
	- Inline functions are probably better, but let's do a define as an exercise
- Look into how to read CHIP8 programs in this C interpreter
	- [DONE] Look if program really is in memory at correct place.
	- [DONE] Can you read first instruction correctly now (is it the first instruction of the IBM logo program?)
	- Can we correctly handle possible failures when reading the program? How to catch this? (Now we simply do a return;)
- [DONE] Implement JUMP 1NNN instruction
- Make generic graphics layer so SDL can be replaced with any other graphics library
	- Will be handy if we port to ESP32
- [DONE] Get rid of nvim not found warnings/errors
- [DONE] Make LOG macro's to do better debugging.
- [DONE] Make dummy programs to call subroutines and return from them
	- You can do this with printf '\xde\xad\xbe\ef' > deadbeef.ch8
- [DONE] Implement 6XNN instruction.
- [DONE] Make implementation for instruction_get() function that gives back callback to instruction to execute; 
- [DONE] Moved all instruction to individual functions in instruction.c.
- [DONE] Generate debug information:
	- [DONE] Write PC
	- [DONE] Write instruction
	- [DONE] Write out all nibbles (to see that they're read correctly)
- [DONE] Make debug function to print out VX registers.
- [DONE] Implement 3XNN, 4XNN, 5XY0, 9XY0 and ANNN.
- [DONE] Implement instruction 0NNN (do not implement) and 7XNN
- [DONE] Add README.md.
- [DONE] Check what happens if program is given as input that does not exist.
	- Also cleaned up read.c/read.h.
- [DONE] Format code and start using formatter.
- [DONE] Implemented FX29 instruction.
- [work-in-progress] Implement DXYN.
	- test edge of screen edge cases
	- try unsetting pixels
	- revisit where I should apply the image width and height scaling: now logic is in sdl and in interpreter
		- I would change the pixel drawing to a rectangle drawing
	- segmentation fault when testing 2 DXYN functions
	- is BIT macro well implemented?
- [DONE] Refactor all NIBBLE macro's to inline functions
- [DONE] Implement FX55 and FX65.
- [work-in-progress] Implement 8XY0 and 8XY1.
	- Implement 8XY1.
	- Test 8XY0 and 8XY1.
- Run cmake differently so the compile_commands.json is always generated at the correct place
	- I need to revise my cmake command
- Find a system to efficiently parse the correct instruction in instruction_get in instruction.c
- How should the program stop?
- Implement ncurses graphics library.
- Put everything in src directory.
- Add decent unit test infrastructure.
