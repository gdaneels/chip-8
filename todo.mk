## TODO

- [DONE] Test adding a instruction to memory and reading it out in the run() function
- Add SDL library support to CMakeLists 
- Look into how CHIP 8 programs are formatted
- Look into how to read CHIP8 programs in this C interpreter
	- [DONE] Look if program really is in memory at correct place.
	- Can you read first instruction correctly now (is it the first instruction of the IBM logo program?)
	- Can we correctly handle possible failures when reading the program? How to catch this? (Now we simply do a return;)
