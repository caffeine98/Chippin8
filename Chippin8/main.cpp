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
	c->opcode_00E0();
	std::cout << c->display[12][12];
	
	delete c;
	return 0;
}