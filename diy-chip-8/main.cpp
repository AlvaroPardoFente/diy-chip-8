#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include "Chip8.h"

const int clock_speed = 500; // Hz
const int update_interval = 1000 / clock_speed; // Period

const int timer_speed = 60; // Hz - Timer frequency
const int update_timer_period = 1000 / timer_speed; // Period

const int defaultWindowWidth = 64 * 20;
const int defaultWindowHeight = 32 * 20;

const int logicalWidth = 64;
const int logicalHeight = 32;

static int updateScreen(SDL_Renderer* renderer, const std::bitset<64 * 32>& gfx)
{
	SDL_SetRenderDrawColor(renderer, 42, 31, 48, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 230, 176, 101, SDL_ALPHA_OPAQUE);
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

static bool handleEvents(Chip8& chip8, SDL_Event& event)
{
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				return false;
			else
				handleKeyPress(chip8, event);
			break;
		case SDL_KEYUP:
			handleKeyRelease(chip8, event);
		default:
			break;
		}
	}

	return true;
}

int main(int argc, char** argv)
{
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Event event{};

	Mix_Chunk* beep = nullptr;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
	{
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		}
		std::cout << "Subsystems Initialised..." << std::endl;

		window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, defaultWindowWidth, defaultWindowHeight, 0);
		if (!window)
			throw std::runtime_error("Window could not be created");

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);
		}
		else
			throw std::runtime_error("Renderer could not be created");

		beep = Mix_LoadWAV("resources/audio/beep.wav");
		if (!beep)
			throw std::runtime_error("Beep file cound not be opened");
	}

	// Game loop time handling
	uint32_t last_update_time = 0;
	int32_t delta_time = 0;

	// Timer handling
	uint32_t last_timer_update = 0;

	// Emulator init
	Chip8 chip8{};
	chip8.init();

	// Emulator game loading

	bool running = true;
	while (running)
	{
		// Update game loop time
		uint32_t current_time = SDL_GetTicks();
		delta_time = current_time - last_update_time;

		// Sleep if needed to keep time per frame
		int32_t time_to_sleep = update_interval - delta_time;
		if (time_to_sleep > 0)
		{
			SDL_Delay(time_to_sleep);
		}

		// Store key press state
		running = handleEvents(chip8, event);

		// Emulate one cycle
		chip8.emulateCycle();

		// If the draw flag is set, update the screen
		if (chip8.drawFlag)
		{
			updateScreen(renderer, chip8.getGfx());
			chip8.drawFlag = false;
		}

		// Update timers
		if (current_time - last_timer_update >= update_timer_period)
		{
			if (chip8.delay_timer > 0)
				--chip8.delay_timer;
			if (chip8.sound_timer > 0)
			{
				if (!(--chip8.sound_timer))
					Mix_PlayChannel(-1, beep, 0);
			}

			last_timer_update = current_time;
		}

		last_update_time = current_time;
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	Mix_FreeChunk(beep);
	Mix_Quit();
	SDL_Quit();

	return 0;
}