#pragma once
#include <array>
#include <bitset>
#include <functional>
#include <random>
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

	// 16 level PC stack
	std::array<uint16_t, 16> stack;

	// Points to the current stack index
	uint16_t stack_pointer;

	// Define the type for the opcode handler functions
	using OpcodeHandler = std::function<void()>;

	// Random number generation
	std::mt19937 gen{ std::random_device{}() };
	std::uniform_int_distribution<unsigned short> dist{ 0, 255 };
	uint8_t random();
public:

	// 60hz timer register. Counts down when above 0
	uint8_t delay_timer;

	// 60hz timer register for sound. The system's buzzer sounds when it reaches 0
	uint8_t sound_timer;

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

	// Default
	void cpuNULL();

	// First level instructions
	void i0x0();
	void i0x1NNN();
	void i0x2NNN();
	void i0x3XKK();
	void i0x4XKK();
	void i0x5XY0();
	void i0x6XKK();
	void i0x7XKK();
	void i0x8();
	void i0x9XY0();
	void i0xANNN();
	void i0xBNNN();
	void i0xCXKK();
	void i0xDXYN();
	void i0xE();
	void i0xF();

	// 0x0 instructions
	void i0x0NNN();
	void i0x00E0();
	void i0x00EE();

	// 0x8 instructions
	void i0x8XY0();
	void i0x8XY1();
	void i0x8XY2();
	void i0x8XY3();
	void i0x8XY4();
	void i0x8XY5();
	void i0x8XY6();
	void i0x8XY7();
	void i0x8XYE();

	// 0xE instructions
	void i0xEX9E();
	void i0xEXA1();

	// 0xF instructions
	void i0xFX07();
	void i0xFX0A();
	void i0xFX15();
	void i0xFX18();
	void i0xFX1E();
	void i0xFX29();
	void i0xFX33();
	void i0xFX55();
	void i0xFX65();
};
