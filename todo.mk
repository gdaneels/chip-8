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
	- you can do this with printf '\xde\xad\xbe\ef' > deadbeef.ch8
- Implement instruction 0NNN (do not implement) and 6XNN and 7XNN
- Put built in font, as defines, together with other defines in interpreter.h
- Generate debug information:
	- Write PC
	- Write instruction
	- Write out all nibbles (to see that they're read correctly)
- Run cmake differently so the compile_commands.json is always generated at the correct place
	- I need to revise my cmake command
- Find a system to efficiently parse the correct instruction
- How should the program stop?

