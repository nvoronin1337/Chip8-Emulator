#pragma once

#ifndef PLARFORM
#define PLATFORM

#include <SDL.h>
#include <cstdint>

class SDL_Platform {
private:
	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
public:
	explicit SDL_Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
	~SDL_Platform();

	void Update(void const* buffer, int pitch);
	bool ProcessInput(uint8_t* keys);
};

#endif