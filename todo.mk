## TODO

- [DONE] Test adding a instruction to memory and reading it out in the run() function
- [DONE] Add SDL library support to CMakeLists 
- Create macro's to fetch first, second, third and forth nibbles of instruction
	- Functions are probably better, but let's do a define as an exercise
- Look into how CHIP 8 programs are formatted
- Look into how to read CHIP8 programs in this C interpreter
	- [DONE] Look if program really is in memory at correct place.
	- Can you read first instruction correctly now (is it the first instruction of the IBM logo program?)
	- Can we correctly handle possible failures when reading the program? How to catch this? (Now we simply do a return;)
- Make generic graphics layer so SDL can be replaced with any other graphics library
	- Will be handy if we port to ESP32
- Get rid of nvim not found warnings/errors
- Put built in font, as defines, together with other defines in interpreter.h
