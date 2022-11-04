#include "emulator.h"
#include "fonts.h"

#include <fstream>
#include <random>
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <time.h>

//CHIP-8 instructions generally start at memory location 0x200
const unsigned int START_ADDRESS = 0x200;	
const unsigned int FONTSET_START_ADDRESS = 0x000;

Chippin8::Chippin8() {
	this->pc = START_ADDRESS;
	
	// Load font in memory
	for (int i = 0; i < sizeof(fontset) / sizeof(fontset[0]); i++) {
		this->memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	// Random number generator for CXNN instruction (rand)
	// Note: Check this code in the future. Maybe C-style rand could be better?
	std::default_random_engine randGen(
		std::chrono::system_clock::now().time_since_epoch().count()
	);
	std::uniform_int_distribution<int> randByte(0, 255U);
	srand(time(NULL));
}

Chippin8::~Chippin8() {

}

void Chippin8::loadROM(std::string filename) {
	// Open ROM file in binary mode and move file pointer to end 
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		// Get file size and allocate buffer of that size
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Fill the buffer with contents of the ROM
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load ROM into Chippin8 memory at memory location START_ADDRESS
		for (long i = 0; i < size; ++i) {
			this->memory[START_ADDRESS + i] = buffer[i];
		}

		// Free buffer
		delete[] buffer;
	}
}

/* ----- Chip-8 Opcodes ----- */

void Chippin8::opcode_0NNN() {
	// This Opcode calls machine instructions for th3 RCA 1802. For this 
	// emulator, it can remain unimplemented. Most CHIP-8 games do not use this 
	// instruction anyway.
}

void Chippin8::opcode_00E0() {
	//Clear screen
	for (int i = 0; i < DISPLAY_WIDTH; i++) {
		for (int j = 0; j < DISPLAY_HEIGHT; j++) {
			this->display[i][j] = this->display[i][j] & 0;
		}
	}
	//memset(this->display, 0, sizeof(display));
}

void Chippin8::opcode_00EE() {
	// To return from a subroutine, we pop the last address from the stack,
	// subtract one from the stack pointer and set the program counter to it.
	this->pc = stack[--sp];
}

void Chippin8::opcode_1NNN() {
	// Jump to address NNN by setting the program counter to last 3 bytes of
	// the opcode. (uint16_t addr = opcode & 0x0FFFu)
	this->pc = opcode & 0x0FFFu;
}

void Chippin8::opcode_2NNN() {
	// Call a subroutine by pushing the program counter to the stack and 
	// setting the program counter to NNN.
	stack[sp] = pc;
	++sp;
	this->pc = opcode & 0x0FFFu;
}

void Chippin8::opcode_3XNN() {
	// If Vx == NN, skip the next instruction.
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t NN = opcode & 0x00FFu;
	if (registers[Vx] == NN) {
		pc += 2;
	}
}

void Chippin8::opcode_4XNN() {
	// If Vx != NN, skip the next instruction
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t NN = opcode & 0x00FFu;
	if (registers[Vx] != NN) {
		pc += 2;
	}
}

void Chippin8::opcode_5XY0() {
	// If Vx == Vy, skip the next instruction
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	if (registers[Vx] == registers[Vy]) {
		pc += 2;
	}
}

void Chippin8::opcode_6XNN() {
	// Set register Vx to NN
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t NN = opcode & 0x00FFu;
	registers[Vx] = NN;
}

void Chippin8::opcode_7XNN() {
	// Add to Vx the value NN
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t NN = opcode & 0x00FFu;
	registers[Vx] += NN;
}

void Chippin8::opcode_8XY0() {
	// Set Vx = Vy
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	registers[Vx] = registers[Vy];
}

void Chippin8::opcode_8XY1() {
	// Set Vx to Vx OR Vy
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	registers[Vx] |= registers[Vy];
}

void Chippin8::opcode_8XY2() {
	// Set Vx to Vx AND Vy
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	registers[Vx] &= registers[Vy];
}

void Chippin8::opcode_8XY3() {
	// Sets Vx to Vx XOR Vy
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	registers[Vx] ^= registers[Vy];
}

void Chippin8::opcode_8XY4() {
	// Add Vx and Vy and store in Vx. Carry is set in VF
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	registers[Vx] += registers[Vy];

	//Overflow occurs if the the value is greater than 8 bits (255)
	registers[0xF] = (registers[Vx] > 255) ? 1 : 0;
}

void Chippin8::opcode_8XY5() {
	// Subtract Vy from Vx and store in Vx. 
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;

	// If there is a borrow, VF register is set to 0. Otherwise it is set to 1.
	registers[0xF] = (registers[Vx] < registers[Vy]) ? 0 : 1;
	
	registers[Vx] -= registers[Vy];
}

void Chippin8::opcode_8XY6() {
	// Store LSB in VF, then bit-shift Vx to the right by one.
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	registers[0xF] = Vx & 0x01u;

	registers[Vx] >>= 1;
}

void Chippin8::opcode_8XY7() {
	// Subtract Vx from Vy and store in Vx
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	
	// If there is a borrow, VF register is set to 0. Otherwise it is set to 1.
	registers[0xF] = (registers[Vy] < registers[Vx]) ? 0 : 1;

	registers[Vx] = registers[Vy] - registers[Vx];
}

void Chippin8::opcode_8XYE() {
	// Store MSB in VF, then bit-shift to the left by one.
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	registers[0xF] = (Vx & 0x80u) >> 7;

	registers[Vx] <<= 1;
}

void Chippin8::opcode_9XY0() {
	// If Vx != Vy, skip the next instruction
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;

	if (registers[Vx] != registers[Vy]) {
		pc += 2;
	}
}

void Chippin8::opcode_ANNN() {
	// Set Index register to NNN
	this->index = opcode & 0x0FFFu;
}

void Chippin8::opcode_BNNN() {
	// Jump to address NNN + V0
	uint8_t NNN = opcode & 0x0FFFu;
	this->pc = NNN + registers[0x0];
}

void Chippin8::opcode_CXNN() {
	// Set Vx to random number (from 0 to 255) AND NN
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t NN = opcode & 0x00FFu;
	uint8_t random = rand() % 256;

	registers[Vx] = random & NN;
}

// @TODO: review this code later
void Chippin8::opcode_DXYN() {
	// Draw a sprite at coordinate (X, Y), with a width of 8 pixels and height of 
	// N pixels. Drawing is done by XORing using a mask to set displaybits on or 
	// off

	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	const uint8_t width = 8; // Sprite width is guaranteed to be 8 pixels wide
	uint8_t height = opcode & 0x000Fu; // Sprite height ranges from 1 to 15

	uint8_t xPosition = registers[Vx] % DISPLAY_WIDTH;
	uint8_t yPosition = registers[Vy] % DISPLAY_HEIGHT;

	registers[0xF] = 0;

	uint32_t mask[DISPLAY_WIDTH][DISPLAY_HEIGHT];
	memcpy(mask, this->display, sizeof(mask));

	// Turn on sprite pixels
	for (int8_t i = 0; i < width; i++) {
		for (int8_t j = 0; j < height; j++) {
			mask[Vx + i][Vy + j] = 0xFFFFFFFFu;
		}
	}

	// XOR mask and display to tun on required pixels
	for (int8_t i = 0; i < width; i++) {
		for (int8_t j = 0; j < height; j++) {
			if (xPosition + i > (DISPLAY_WIDTH - 1) || yPosition + j > (DISPLAY_HEIGHT - 1)) {
				break;
			}
			uint32_t bit = this->display[xPosition + i][yPosition + j];
			this->display[xPosition + i][yPosition + j] ^= mask[i][j];
			// Set VF to 1 if a bit gets set to zero (Note: messy solution, will update)
			if (bit == 1 && this->display[i][j] == 0) {
				registers[0xF] = 1;
			}
		}
	}
}

void Chippin8::opcode_EX9E() {
	// If key Vx is pressed, skip the next instruction
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	
	if (keypad[registers[Vx]]) {
		pc += 2;
	}
}

void Chippin8::opcode_EXA1() {

}

void Chippin8::opcode_FX07() {

}

void Chippin8::opcode_FX0A() {

}

void Chippin8::opcode_FX15() {

}

void Chippin8::opcode_FX18() {

}

void Chippin8::opcode_FX1E() {

}

void Chippin8::opcode_FX29() {

}

void Chippin8::opcode_FX33() {

}

void Chippin8::opcode_FX55() {

}

void Chippin8::opcode_FX65() {

}