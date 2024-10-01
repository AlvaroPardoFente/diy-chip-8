#include <SDL.h>
#include <iostream>
#include "Chip8.h"

const int defaultWindowWidth = 1080;
const int defaultWindowHeight = 720;

const int logicalWidth = 1080;
const int logicalHeight = 720;

int main(int argc, char** argv)
{
	SDL_Window* window{};
	SDL_Renderer* renderer{};

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "Subsystems Initialised..." << std::endl;

		window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, defaultWindowWidth, defaultWindowHeight, 0);
		if (window)
		{
			std::cout << "Window created!" << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);
			std::cout << "Renderer created!" << std::endl;
		}
	}

	// Emulator init
	Chip8 chip8{};
	chip8.init();

	// Emulator game loading

	bool running = true;
	while (running)
	{
		// Emulate one cycle
		running = chip8.emulateCycle();

		// If the draw flag is set, update the screen
		if (chip8.drawFlag)
		{
		}

		// Store key press state
	}

	return 0;
}