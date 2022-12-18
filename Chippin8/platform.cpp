#include "platform.h"
#include <string>
#include <stdint.h>
#include <cstdlib>
#include <iostream>

Platform::Platform(std::string title, int width, int height, int tWidth, int tHeight) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"SDL Error",
			"Could not Initialize SDL",
			NULL
		);
	}
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)
		(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	
	window = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		width, 
		height,
		windowFlags);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	texture = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_STREAMING, 
		tWidth,  
		tHeight);
}

Platform::~Platform() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch) {
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool Platform::ProcessInputs(uint8_t* keypad) {
	bool isRunning = true;

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			isRunning = false;
		}

		/*
			Key Bindings:
			1  2  3  4		 keypad[1] keypad[2] keypad[3] keypad[C]		
			Q  W  E  R	-->	 keypad[4] keypad[5] keypad[6] keypad[D]
			A  S  D  F		 keypad[7] keypad[8] keypad[9] keypad[E]
			Z  X  C  V		 keypad[A] keypad[0] keypad[B] keypad[F]
		*/

		switch (event.type) {
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					isRunning = false;
					break;

				case SDLK_x:
					keypad[0] = 1;
					break;

				case SDLK_1:
					keypad[1] = 1;
					break;

				case SDLK_2:
					keypad[2] = 1;
					break;

				case SDLK_3:
					keypad[3] = 1;
					break;

				case SDLK_q:
					keypad[4] = 1;
					break;

				case SDLK_w:
					keypad[5] = 1;
					break;

				case SDLK_e:
					keypad[6] = 1;
					break;

				case SDLK_a:
					keypad[7] = 1;
					break;

				case SDLK_s:
					keypad[8] = 1;
					break;

				case SDLK_d:
					keypad[9] = 1;
					break;

				case SDLK_z:
					keypad[0xA] = 1;
					break;

				case SDLK_c:
					keypad[0xB] = 1;
					break;

				case SDLK_4:
					keypad[0xC] = 1;
					break;

				case SDLK_r:
					keypad[0xD] = 1;
					break;

				case SDLK_f:
					keypad[0xE] = 1;
					break;

				case SDLK_v:
					keypad[0xF] = 1;
					break;
				}
				break;
			}
			case SDL_KEYUP: 
			{
				switch (event.key.keysym.sym) {
				case SDLK_x:
					keypad[0] = 0;
					break;

				case SDLK_1:
					keypad[1] = 0;
					break;

				case SDLK_2:
					keypad[2] = 0;
					break;

				case SDLK_3:
					keypad[3] = 0;
					break;

				case SDLK_q:
					keypad[4] = 0;
					break;

				case SDLK_w:
					keypad[5] = 0;
					break;

				case SDLK_e:
					keypad[6] = 0;
					break;

				case SDLK_a:
					keypad[7] = 0;
					break;

				case SDLK_s:
					keypad[8] = 0;
					break;

				case SDLK_d:
					keypad[9] = 0;
					break;

				case SDLK_z:
					keypad[0xA] = 0;
					break;

				case SDLK_c:
					keypad[0xB] = 0;
					break;

				case SDLK_4:
					keypad[0xC] = 0;
					break;

				case SDLK_r:
					keypad[0xD] = 0;
					break;

				case SDLK_f:
					keypad[0xE] = 0;
					break;

				case SDLK_v:
					keypad[0xF] = 0;
					break;
				}
				break;
			}
		}
	}
	return isRunning;
}