/*
	Chippin8 - A Chip-8 emulator
	This project is meant to emulate a CHIP-8 system. It should have the 
	ability to run a CHIP-8 ROM, display graphics and play audio. External
	libraries such as SDL will be used.
*/

#include "emulator.h"

#include <iostream>

int main(int argc, char** argv) {
	std::cout << "Chippin in\n";

	Chippin8* c = new Chippin8();
	c->opcode_00E0();	// Clear screen
	c->loadROM("D:\\Programming stuff\\CPractice\\IBM_Logo.ch8");
	c->opcode_DXYN();
	
	for (int i = 0; i < DISPLAY_WIDTH; i++) {
		for (int j = 0; j < DISPLAY_HEIGHT; j++) {
			std::cout << c->display[i][j] << ", ";
			if (i % 8 == 0) std::cout << std::endl;
		}
	}
	
	delete c;
	return 0;
}