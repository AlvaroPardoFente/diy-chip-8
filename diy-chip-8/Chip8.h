#pragma once
#include <array>
#include <bitset>
#include <functional>
class Chip8
{
private:
	// Currently running opcode
	uint16_t opcode;

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

	// Define the type for the opcode handler functions
	using OpcodeHandler = std::function<void()>;

public:

	std::array<bool, 16> keyState;

	const std::bitset<64 * 32>& getGfx();

	bool drawFlag{};

	int init();

	bool emulateCycle();

	// Instruction implementation
private:
	// Init all jump tables with function pointers
	void initJumpTables();

	// Array to store opcode handlers
	std::array<OpcodeHandler, 16> firstWordInstructions{}; // 16-bit opcodes

	void cpuNULL();
	void handle0xA000();
};
