#include <SDL.h>
#include <iostream>
#include "Chip8.h"

const int defaultWindowWidth = 1080;
const int defaultWindowHeight = 720;

const int logicalWidth = 64;
const int logicalHeight = 32;

int updateScreen(SDL_Renderer* renderer, const std::bitset<64 * 32>& gfx)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			if (gfx.test(i * 64 + j))
			{
				SDL_RenderDrawPoint(renderer, j, i);
			}
		}
	}

	SDL_RenderPresent(renderer);

	return 0;
}

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
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);
			std::cout << "Renderer created!\n";
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
			updateScreen(renderer, chip8.getGfx());
		}

		// Store key press state
	}

	// Test screen
	std::bitset<64 * 32> btest{};
	btest.set(2);
	btest.set(30);
	btest.set(31);
	btest.set(70);
	btest.set(100);
	btest.set(64 * 10);

	updateScreen(renderer, btest);
	std::cin.get();

	return 0;
}