<h1 align="center">C CHIP-8 emulator</h1>
My own implementation of the CHIP-8 emulator, based on the <a href="https://tobiasvl.github.io/blog/write-a-chip-8-emulator/">"Guide to making a CHIP-8 emulator"</a>, written by Tobias V. Langhoff. 
<br/>
<br/>
<p align="center"><a href="https://en.wikipedia.org/wiki/CHIP-8"><img width="50%" src="https://upload.wikimedia.org/wikipedia/commons/thumb/5/54/Space_intercept.png/1920px-Space_intercept.png" /></a></p>

### Build

To build the emulator, run following commands sequentially in the main directory:
```
mkdir build
cd build
cmake ..
make
```

### Run

To run the emulator, run following command in the `build` directory:
```
./chip8 ../programs/name-of-program.ch8
```

### Implementation status

- [x] Test adding a instruction to memory and reading it out in the run() function
- [x] Add SDL library support to CMakeLists 
- [x] Create macro's to fetch first, second, third and forth nibbles of instruction
	- Inline functions are probably better, but let's do a define as an exercise
- [ ] Look into how to read CHIP8 programs in this C interpreter
	- [x] Look if program really is in memory at correct place.
	- [x] Can you read first instruction correctly now (is it the first instruction of the IBM logo program?)
	- Can we correctly handle possible failures when reading the program? How to catch this? (Now we simply do a return;)
- [x] Implement JUMP 1NNN instruction_get- Make generic graphics layer so SDL can be replaced with any other graphics library
	- Will be handy if we port to ESP32
- [x] Get rid of nvim not found warnings/errors
- [x] Make LOG macro's to do better debugging.
- [x] Make dummy programs to call subroutines and return from them
	- You can do this with printf '\xde\xad\xbe\ef' > deadbeef.ch8
- [x] Implement 6XNN instruction.
- [x] Make implementation for instruction_get() function that gives back callback to instruction to execute; 
- [x] Moved all instruction to individual functions in instruction.c.
- [x] Generate debug information:
	- [x] Write PC
	- [x] Write instruction
	- [x] Write out all nibbles (to see that they're read correctly)
- [x] Make debug function to print out VX registers.
- [x] Implement 3XNN, 4XNN, 5XY0, 9XY0 and ANNN.
- [x] Implement instruction 0NNN (do not implement) and 7XNN
- [x] Add README.md.
- [x] Check what happens if program is given as input that does not exist.
	- Also cleaned up read.c/read.h.
- [x] Format code and start using formatter.
- [x] Implemented FX29 instruction.
- [ ] [work-in-progress] Implement DXYN.
	- test edge of screen edge cases
	- try unsetting pixels
	- revisit where I should apply the image width and height scaling: now logic is in sdl and in interpreter
		- I would change the pixel drawing to a rectangle drawing
	- segmentation fault when testing 2 DXYN functions
	- is BIT macro well implemented?
- [x] Refactor all NIBBLE macro's to inline functions
- [x] Implement FX55 and FX65.
- [ ] [work-in-progress] Test 8XYx instructions.
- [ ] [work-in-progress] Implement 8XY6 and 8XYE.
	- Implement.
	- Test.
- [ ] Run cmake differently so the compile_commands.json is always generated at the correct place
	- I need to revise my cmake command
- [ ] Find a system to efficiently parse the correct instruction in instruction_get in instruction.c
- [ ] How should the program stop?
- [ ] Implement ncurses graphics library.
- [ ] Put everything in src directory.
- [ ] Add decent unit test infrastructure.
