#pragma once
#include <array>
#include <bitset>
class Chip8
{
private:
	// Currently running opcode
	uint16_t current_opcode;

	// Chip-8 RAM
	std::array<uint8_t, 4096> memory;

	// Processor registers
	std::array<uint8_t, 16> V;

	// Memory index register
	uint16_t I;

	// Program counter
	uint16_t PC;

	// Pixel state of the screen
	std::bitset<64 * 32> gfx;

	// 60hz timer register. Counts down when above 0
	uint8_t delay_timer;

	// 60hz timer register for sound. The system's buzzer sounds when it reaches 0
	uint8_t sound_timer;

	// 16 level PC stack
	std::array<uint16_t, 16> stack;

	// Points to the current stack index
	uint16_t stack_pointer;

	std::array<uint8_t, 16> chip8_fontset;

public:
	bool drawFlag{};

	int init();

	bool emulateCycle();
};
