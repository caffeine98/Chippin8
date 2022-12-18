/*
	 _____ _     _             _       _____
	/  __ \ |   (_)           (_)     |  _  |
	| /  \/ |__  _ _ __  _ __  _ _ __  \ V /
	| |   | '_ \| | '_ \| '_ \| | '_ \ / _ \
	| \__/\ | | | | |_) | |_) | | | | | |_| |
	 \____/_| |_|_| .__/| .__/|_|_| |_\_____/
				  | |   | |
				  |_|   |_|

	Chippin8 is an emulator for a CHIP-8. It is technically an interpreted
	programming language that was used on old microcomputers such as the
	COSMAC VIP and Telmac-1800 from the 70s.

	Currently, this emulator is capable of running CHIP-8 roms (.ch8 file), 
	which is supplied to the emulator via command line argument.
		./<Chippin8.exe> <ROM_file.ch8>

	This project uses SDL2 to display the programs as well as for keyboard 
	input.
	
	The following keys act as keypad input for the system:
			1  2  3  4		 keypad[1] keypad[2] keypad[3] keypad[C]
			Q  W  E  R	-->	 keypad[4] keypad[5] keypad[6] keypad[D]
			A  S  D  F		 keypad[7] keypad[8] keypad[9] keypad[E]
			Z  X  C  V		 keypad[A] keypad[0] keypad[B] keypad[F]

	Sound support is not added yet, but may be added in the future.
*/

#include "emulator.h"
#include "platform.h"

#include <SDL.h>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define USAGE() do{ \
		std::cout << "Usage: ./<Chippin8>.exe <ROM_file>.ch8"\
		<< " [Video Scale (number)] [Cycle Delay {number}]\n"; \
		} while(0)

// Maximum size is limited to prevent user from creating a ginormous window
const int MAXIMUM_VIDEO_SCALE = 25; 

// Check if argument is a number https://stackoverflow.com/a/17976083
bool isNumber(std::string& s) {
	return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

int main(int argc, char* argv[]) {
	int videoScale = 10;		// Default value
	uint32_t cycleDelay = 5;	// Default value

	if (argc == 1) {
		USAGE();
		return EXIT_FAILURE;
	}
	
	std::string ROMFile = argv[1];
	fs::path ROMFilePath(ROMFile);
	if (!fs::exists(ROMFilePath)) {
		std::cerr << "Invalid CHIP-8 ROM file enetered\n";
		return EXIT_FAILURE;
	}

	if (argc > 2) {
		// Check if user entered a video scale value
		if (argv[2]) {
			std::string videoScaleStr = argv[2];
			if (isNumber(videoScaleStr)) {
				videoScale = MIN(abs(std::stoi(videoScaleStr)), 
								MAXIMUM_VIDEO_SCALE);
				std::cout << "Video Scale: " << videoScale << '\n';
			}
			else {
				std::cout << "Invalid video scale. Using defaiult value: "
					<< videoScale << '\n';
			}
		}

		// Check if user entered a Cycle Delay (speed) value
		if (argv[3]) {
			std::string cycleDelayStr = argv[3];
			std::cout << cycleDelayStr << '\n';
			if (isNumber(cycleDelayStr)) {
				cycleDelay = (uint32_t) abs(std::stoi(cycleDelayStr));
				std::cout << "Cycle Delay: " << cycleDelay << '\n';
			}
		}
	}

	std::cout << "Launching Chippin8\n";
	std::cout << "Running " << ROMFile << '\n';

	Platform platform("Chippin8", DISPLAY_WIDTH * videoScale, 
		DISPLAY_HEIGHT * videoScale, DISPLAY_WIDTH, DISPLAY_HEIGHT
	);

	Chippin8 c8;
	c8.LoadROM(ROMFile);
	
	int videoPitch = sizeof(c8.display[0]) * DISPLAY_WIDTH;
	bool isRunning = true;

	while (isRunning) {
		isRunning = platform.ProcessInputs(c8.keypad);

		c8.Cycle();
		platform.Update(c8.display, videoPitch);
		
		SDL_Delay(cycleDelay);
	}
	
	return EXIT_SUCCESS;
}