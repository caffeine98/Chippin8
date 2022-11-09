/*
	Chippin8 - A Chip-8 emulator
	Here, we will define the system's components, using internet resources as 
	a guide
*/

#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <string>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

class Chippin8 {
public:
	/* ----- System components ----- */
	uint8_t memory[4096];	 // 4KB memory.
	uint32_t display[DISPLAY_WIDTH][DISPLAY_HEIGHT]; // 64 x 32 pixel display
	uint16_t opcode;		 // Opcode
	uint8_t pc;				 // Program counter
	uint16_t index;			 // Index register. Points at locations in memory
	uint8_t registers[16];	 // 16 8-bit registers (V0 - VF)
	uint16_t stack[16];		 // Stack for storing 16-bit addresses
	uint8_t sp;				 // Stack pointer
	uint8_t delayTimer;		 // Used for timing events of games
	uint8_t soundTimer;		 // Used for sound effects. Beeps at non-zero values
	uint8_t keypad[16];		 // Store keypad values

	Chippin8();
	~Chippin8();

	/* ----- System Functionality ----- */

	// Load ROM file 
	void LoadROM(std::string filename);

	// Instruction Cycle (Fetch, Decode, Execute)
	void Cycle();
	
	// Decode opcode and call instruction function
	void DecodeAndExecute(uint16_t opcode);

	/* ----- CHIP - 8 Instructions ----- */
	/*
		Implementing the Opcode table from wikipedia: 
		https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
	*/

	// NOP instruction. Does nothing
	void opcode_NOP();

	// Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. 
	// Not necessary for most ROMs.
	void opcode_0NNN();

	// Clears the screen
	void opcode_00E0();

	// Returns from a subroutine
	void opcode_00EE();

	// Jump to address NNN
	void opcode_1NNN();

	// Calls subroutine at NNN
	void opcode_2NNN();

	// Skips the next instruction if VX equals NN (usually the next instruction
	// is a jump to skip a code block).
	void opcode_3XNN();

	// Skips the next instruction if VX does not equal NN (usually the next 
	// instruction is a jump to skip a code block).
	void opcode_4XNN();

	// Skips the next instruction if VX equals VY (usually the next instruction
	// is a jump to skip a code block).
	void opcode_5XY0();

	// Sets VX to NN
	void opcode_6XNN();

	// Adds NN to VX (carry flag is not changed).
	void opcode_7XNN();

	// Sets VX to the value of VY.
	void opcode_8XY0();

	// Sets VX to VX or VY. (bitwise OR operation)
	void opcode_8XY1();

	// Sets VX to VX and VY. (bitwise AND operation)
	void opcode_8XY2();

	// Sets VX to VX xor VY.
	void opcode_8XY3();

	// Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there
	// is not.
	void opcode_8XY4();

	// VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 
	// when there is not.
	void opcode_8XY5();

	// Stores the least significant bit of VX in VF and then shifts VX to the 
	// right by 1.
	void opcode_8XY6();

	// Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 
	// when there is not.
	void opcode_8XY7();

	// Stores the most significant bit of VX in VF and then shifts VX to the 
	// left by 1.
	void opcode_8XYE();

	// Skips the next instruction if VX does not equal VY. (Usually the next 
	// instruction is a jump to skip a code block)
	void opcode_9XY0();

	// Sets I to the address NNN.
	void opcode_ANNN();

	// Jumps to the address NNN plus V0.
	void opcode_BNNN();

	// Sets VX to the result of a bitwise and operation on a random number 
	// (Typically: 0 to 255) and NN.
	void opcode_CXNN();

	// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a
	// height of N pixels. Each row of 8 pixels is read as bit-coded starting 
	// from memory location I; I value does not change after the execution of 
	// this instruction. As described above, VF is set to 1 if any screen 
	// pixels are flipped from set to unset when the sprite is drawn, and to 0 
	// if that does not happen.
	void opcode_DXYN();

	// Skips the next instruction if the key stored in VX is pressed (usually 
	// the next instruction is a jump to skip a code block).
	void opcode_EX9E();

	// Skips the next instruction if the key stored in VX is not pressed 
	// (usually the next instruction is a jump to skip a code block).
	void opcode_EXA1();

	// Sets VX to the value of the delay timer.
	void opcode_FX07();

	// A key press is awaited, and then stored in VX (blocking operation, all 
	// instruction halted until next key event).
	void opcode_FX0A();

	// Sets the delay timer to VX.
	void opcode_FX15();

	// Sets the sound timer to VX.
	void opcode_FX18();

	// Adds VX to I. VF is not affected.
	void opcode_FX1E();

	// Sets I to the location of the sprite for the character in VX. Characters
	// 0-F (in hexadecimal) are represented by a 4x5 font.
	void opcode_FX29();

	// Stores the binary-coded decimal representation of VX, with the hundreds 
	// digit in memory at location in I, the tens digit at location I+1, and 
	// the ones digit at location I+2.
	void opcode_FX33();

	// Stores from V0 to VX (including VX) in memory, starting at address I. 
	// The offset from I is increased by 1 for each value written, but I 
	// itself is left unmodified.
	void opcode_FX55();

	// Fills from V0 to VX (including VX) with values from memory, starting at 
	// address I. The offset from I is increased by 1 for each value read, but 
	// I itself is left unmodified.
	void opcode_FX65();

private:
	
};

#endif // EMULATOR_H