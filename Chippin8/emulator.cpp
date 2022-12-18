#include "emulator.h"
#include "fonts.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

//CHIP-8 instructions generally start at memory location 0x200
const uint16_t START_ADDRESS = 0x200;	
const uint16_t FONTSET_START_ADDRESS = 0x000;

Chippin8::Chippin8() {
	// Set Program Counter starting position
	pc = START_ADDRESS;
	
	// Load font in memory
	for (int i = 0; i < sizeof(fontset) / sizeof(fontset[0]); i++) {
		this->memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	// Random number seed for CXNN instruction 
	srand(time(NULL));

	//Clear Screen initially
	opcode_00E0();

	// Clear keypad input values
	for (int i = 0; i < 16; ++i) {
		keypad[i] = 0;
	}
}

Chippin8::~Chippin8() {

}

void Chippin8::LoadROM(std::string filename) {
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

		delete[] buffer;

//#define DEBUG_MEMORY_CONTENTS
#ifdef DEBUG_MEMORY_CONTENTS
		std::cout << std::hex 
			<< "-- Start of fonts (0x" << FONTSET_START_ADDRESS << ") --";
		for (long i = 0; i < (long)size + 0x200; ++i) {
			if (i % 8 == 0) std::cout << std::endl;
			if (i == 0x200) std::cout << std::hex 
				<< "-- Start of ROM (0x" << START_ADDRESS << ") --\n";
			std::cout << std::hex << i << "\t" << (int)memory[i] 
				<< std::dec << " ";
		}
		std::cout << "\n-- End of ROM --\n";
#endif // DEBUG_MEMORY_CONTENTS
	}
}

void Chippin8::Cycle() {
	// * Fetch
	// Opcode is 16 bytes, so the first 8 bytes pointed at by the program 
	// counter in memory, while the next 8 bytes are stored at pc + 1.
	this->opcode = (memory[pc] << 8) | memory[pc + 1];
	//std::cout << std::hex << opcode << '\n' << std::dec;
	pc += 2;	// Move program counter to the next instruction in memory.

	// * Decode and Execute
	DecodeAndExecute(opcode);

	// Decrement delayTimer and soundTimer
	if (delayTimer > 0) { --delayTimer; }
	if (soundTimer > 0) { --soundTimer; }
}

//#define DEBUG_DECODE_AND_EXECUTE
#ifdef DEBUG_DECODE_AND_EXECUTE
#define RUN_OPCODE(x) do{x(); std::cout << #x << "\n";} while(0)
#else
#define RUN_OPCODE(x) x()
#endif // DEBUG_DECODE_AND_EXECUTE

void Chippin8::DecodeAndExecute(uint16_t opcode) {
	switch ((opcode & 0xF000) >> 12) {
	case 0x0:
		// Note: no need to decode 0NNN instruction
		switch ((opcode & 0x000F)) {
		case 0x0:
			RUN_OPCODE(opcode_00E0);
			break;
		case 0xE:
			RUN_OPCODE(opcode_00EE);
			break;
		}
		break;
	
	case 0x1:
		RUN_OPCODE(opcode_1NNN);
		break;
	
	case 0x2:
		RUN_OPCODE(opcode_2NNN);
		break;
	
	case 0x3:
		RUN_OPCODE(opcode_3XNN);
		break;
	
	case 0x4:
		RUN_OPCODE(opcode_4XNN);
		break;
	
	case 0x5:
		RUN_OPCODE(opcode_5XY0);
		break;
	
	case 0x6:
		RUN_OPCODE(opcode_6XNN);
		break;
	
	case 0x7:
		RUN_OPCODE(opcode_7XNN);
		break;
	
	case 0x8:
		switch (opcode & 0x000F) {
		case 0x0:
			RUN_OPCODE(opcode_8XY0);
			break;
		case 0x1:
			RUN_OPCODE(opcode_8XY1);
			break;
		case 0x2:
			RUN_OPCODE(opcode_8XY2);
			break;
		case 0x3:
			RUN_OPCODE(opcode_8XY3);
			break;
		case 0x4: 
			RUN_OPCODE(opcode_8XY4);
			break;
		case 0x5:
			RUN_OPCODE(opcode_8XY5);
			break;
		case 0x6:
			RUN_OPCODE(opcode_8XY6);
			break;
		case 0x7:
			RUN_OPCODE(opcode_8XY7);
			break;
		case 0xE:
			RUN_OPCODE(opcode_8XYE);
			break;
		}
		break;
	
	case 0x9:
		RUN_OPCODE(opcode_9XY0);
		break;
	
	case 0xA:
		RUN_OPCODE(opcode_ANNN);
		break;
	
	case 0xB:
		RUN_OPCODE(opcode_BNNN);
		break;
	
	case 0xC:
		RUN_OPCODE(opcode_CXNN);
		break;
	
	case 0xD:
		RUN_OPCODE(opcode_DXYN);
		break;
	
	case 0xE:
		switch (opcode & 0x00FF) {
		case 0x9E:
			RUN_OPCODE(opcode_EX9E);
			break;
		case 0xA1:
			RUN_OPCODE(opcode_EXA1);
			break;
		}
		break;
	
	case 0xF:
		switch (opcode & 0x00FF) {
		case 0x07:
			RUN_OPCODE(opcode_FX07);
			break;
		case 0x0A:
			RUN_OPCODE(opcode_FX0A);
			break;
		case 0x15:
			RUN_OPCODE(opcode_FX15);
			break;
		case 0x18:
			RUN_OPCODE(opcode_FX18);
			break;
		case 0x1E:
			RUN_OPCODE(opcode_FX1E);
			break;
		case 0x29:
			RUN_OPCODE(opcode_FX29);
			break;
		case 0x33:
			RUN_OPCODE(opcode_FX33);
			break;
		case 0x55:
			RUN_OPCODE(opcode_FX55);
			break;
		case 0x65:
			RUN_OPCODE(opcode_FX65);
			break;
		}
		break;

	default: RUN_OPCODE(opcode_NOP); break;
	}
}

/* ----- CHIP - 8 Instructions ----- */

void Chippin8::opcode_NOP() { /* Do nothing */ }

void Chippin8::opcode_0NNN() {
	// This Opcode calls machine instructions for the RCA 1802. For this 
	// emulator, it can remain unimplemented. Most CHIP-8 games do not use this
	// instruction anyway.
}

void Chippin8::opcode_00E0() {
	//Clear screen
	for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
		display[i] = 0;
	}
}

void Chippin8::opcode_00EE() {
	// To return from a subroutine, we pop the last address from the stack,
	// subtract one from the stack pointer and set the program counter to it.
	pc = stack[--sp];
}

void Chippin8::opcode_1NNN() {
	// Jump to address NNN by setting the program counter to last 3 bytes of
	// the opcode. (uint16_t addr = opcode & 0x0FFFu)
	pc = opcode & 0x0FFFu;
}

void Chippin8::opcode_2NNN() {
	// Call a subroutine by pushing the program counter to the stack and 
	// setting the program counter to NNN.
	stack[sp] = pc;
	++sp;
	pc = opcode & 0x0FFFu;
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
	index = opcode & 0x0FFFu;
}

void Chippin8::opcode_BNNN() {
	// Jump to address NNN + V0
	uint8_t NNN = opcode & 0x0FFFu;
	pc = NNN + registers[0x0];
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

	// Iterate through the sprite pixels. Note that "index" will point to the 
	// the location of the sprite values in memory
	for (int i = 0; i < height; i++) {
		uint8_t sprite = memory[index + i];

		for (int j = 0; j < width; j++) {
			// (0x80u >> j) scans through the width of the pixels at every
			// iteration. It is ANDed with the value of sprite to get the 
			// value of the pixel
			uint8_t spritePixel = sprite & (0x80u >> j); 
			
			uint32_t* screenPixel 
				= &display[(yPosition + i) * DISPLAY_WIDTH + (xPosition + j)];

			if (spritePixel) {
				// If a collision occurs between the sprite pixel and screen 
				// pixel, set register VF to 1
				if ((*screenPixel) == 0xFFFFFFFF) {
					registers[0xF] = 1;
				}
				// XOR sprite pixel with screen pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chippin8::opcode_EX9E() {
	// If key Vx is pressed, skip the next instruction
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	
	if (keypad[registers[Vx]]) {
		pc += 2;
	}
}

void Chippin8::opcode_EXA1() {
	//If key Vx is not pressed, skip the next instruction
	uint8_t Vx = (opcode & 0x0F00u) >> 8;

	if (!keypad[registers[Vx]]) {
		pc += 2;
	}
}

void Chippin8::opcode_FX07() {
	// Set register Vx to the delay timer
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	registers[Vx] = delayTimer;
}

void Chippin8::opcode_FX0A() {
	// Wait for a key press and store its value in Vx after receiving it.
	uint8_t Vx = (opcode & 0x0F00u) >> 8;

	// This instruction should block execution, unless it receives a key press.
	// Waiting can be done by subtracting the program counter. The idea here is
	// that, unless a key press is recieved, we do not allow the program 
	// counter to advance to the next instruction, thus constantly waiting.
	for (int i = 0; i < sizeof(keypad) / sizeof(keypad[0]); i++) {
		if (keypad[i]) {
			registers[Vx] = keypad[i];
			return;
		}
	}
	pc -= 2;
}

void Chippin8::opcode_FX15() {
	// Set the delay timer to Vx
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	delayTimer = registers[Vx];
}

void Chippin8::opcode_FX18() {
	// Set the delay timer to Vx
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	soundTimer = registers[Vx];
}

void Chippin8::opcode_FX1E() {
	// Add Vx to Index register
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	index += registers[Vx];
}

void Chippin8::opcode_FX29() {
	// Set the index register to the location of the sprite stored in Vx
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	
	// Each character is described by 5 bytes, so there is an offset of 5 for 
	// each character
	index = FONTSET_START_ADDRESS + (registers[Vx] * 5);
}

void Chippin8::opcode_FX33() {
	// Store BCD representation of Vx
	// Vx[hundreds] at I, Vx[Tens] at I+1, Vx[Ones] at I+2
	uint8_t Vx = (opcode & 0x0F00u) >> 8;

	memory[index] = (registers[Vx] / 100) % 10;
	memory[index + 1] = (registers[Vx] / 10) % 10;
	memory[index + 2] = registers[Vx] % 10;
}

void Chippin8::opcode_FX55() {
	// Store to memory values from V0 to Vx
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	
	for (int i = 0; i <= Vx; ++i) {
		memory[index + i] = registers[i];
	}
}

void Chippin8::opcode_FX65() {
	// Fill registers V0 to Vx values from memory
	uint8_t Vx = (opcode & 0x0F00u) >> 8;

	for (int i = 0; i < Vx; ++i) {
		registers[i] = memory[index + i];
	}
}