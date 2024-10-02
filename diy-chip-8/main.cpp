#include <SDL.h>
#include <iostream>
#include "Chip8.h"

const int defaultWindowWidth = 1080;
const int defaultWindowHeight = 720;

const int logicalWidth = 64;
const int logicalHeight = 32;

static int updateScreen(SDL_Renderer* renderer, const std::bitset<64 * 32>& gfx)
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

static void handleKeyPress(Chip8& chip8, SDL_Event& event)
{
	switch (event.key.keysym.sym)
	{
	case SDLK_1:
		chip8.keyState[0x1] = true;
		break;
	case SDLK_2:
		chip8.keyState[0x2] = true;
		break;
	case SDLK_3:
		chip8.keyState[0x3] = true;
		break;
	case SDLK_4:
		chip8.keyState[0xC] = true;
		break;
	case SDLK_q:
		chip8.keyState[0x4] = true;
		break;
	case SDLK_w:
		chip8.keyState[0x5] = true;
		break;
	case SDLK_e:
		chip8.keyState[0x6] = true;
		break;
	case SDLK_r:
		chip8.keyState[0xD] = true;
		break;
	case SDLK_a:
		chip8.keyState[0x7] = true;
		break;
	case SDLK_s:
		chip8.keyState[0x8] = true;
		break;
	case SDLK_d:
		chip8.keyState[0x9] = true;
		break;
	case SDLK_f:
		chip8.keyState[0xE] = true;
		break;
	case SDLK_z:
		chip8.keyState[0xA] = true;
		break;
	case SDLK_x:
		chip8.keyState[0x0] = true;
		break;
	case SDLK_c:
		chip8.keyState[0xB] = true;
		break;
	case SDLK_v:
		chip8.keyState[0xF] = true;
		break;
	default:
		break;
	}
}

static void handleKeyRelease(Chip8& chip8, SDL_Event& event)
{
	switch (event.key.keysym.sym)
	{
	case SDLK_1:
		chip8.keyState[0x1] = false;
		break;
	case SDLK_2:
		chip8.keyState[0x2] = false;
		break;
	case SDLK_3:
		chip8.keyState[0x3] = false;
		break;
	case SDLK_4:
		chip8.keyState[0xC] = false;
		break;
	case SDLK_q:
		chip8.keyState[0x4] = false;
		break;
	case SDLK_w:
		chip8.keyState[0x5] = false;
		break;
	case SDLK_e:
		chip8.keyState[0x6] = false;
		break;
	case SDLK_r:
		chip8.keyState[0xD] = false;
		break;
	case SDLK_a:
		chip8.keyState[0x7] = false;
		break;
	case SDLK_s:
		chip8.keyState[0x8] = false;
		break;
	case SDLK_d:
		chip8.keyState[0x9] = false;
		break;
	case SDLK_f:
		chip8.keyState[0xE] = false;
		break;
	case SDLK_z:
		chip8.keyState[0xA] = false;
		break;
	case SDLK_x:
		chip8.keyState[0x0] = false;
		break;
	case SDLK_c:
		chip8.keyState[0xB] = false;
		break;
	case SDLK_v:
		chip8.keyState[0xF] = false;
		break;
	default:
		break;
	}
}

static void handleEvents(Chip8& chip8, SDL_Event& event)
{
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			exit(0);
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				exit(0);
			else
				handleKeyPress(chip8, event);
			break;
		case SDL_KEYUP:
			handleKeyRelease(chip8, event);
		default:
			break;
		}
	}
}

int main(int argc, char** argv)
{
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Event event{};

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
		// Store key press state
		handleEvents(chip8, event);

		// Emulate one cycle
		//running = chip8.emulateCycle();

		// If the draw flag is set, update the screen
		if (chip8.drawFlag)
		{
			updateScreen(renderer, chip8.getGfx());
		}

		//std::copy(chip8.keyState.begin(), chip8.keyState.end(), std::ostream_iterator<int>(std::cout, " "));
		//std::cout << std::endl;
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