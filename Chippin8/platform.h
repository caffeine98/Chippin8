/*
	The emulator is rendered using SDL2. All SDL related code goes here, 
	including rendering and key inputs
*/

#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL.h>
#include <string>
#include <stdint.h>

class Platform {
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	

public:
	Platform(std::string title, int width, int height, int tWidth, int tHeight);
	~Platform();

	void Update(const void* buffer, int pitch);
	bool ProcessInputs(uint8_t* keys);

};

#endif // PLATFORM_H