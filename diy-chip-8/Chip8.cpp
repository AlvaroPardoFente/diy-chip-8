#include "Chip8.h"
#include <iostream>
#include <fstream>

const std::array<uint8_t, 80> chip8_fontset =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int Chip8::init()
{
	PC = 0x200;
	current_opcode = 0;
	I = 0;
	stack_pointer = 0;

	// Clear display
	gfx.reset();

	// Clear stack
	stack.fill(0);
	// Clear registers
	V.fill(0);
	// Clear memory
	memory.fill(0);

	// Init fontset
	std::copy(chip8_fontset.begin(), chip8_fontset.end(), memory.begin());

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	std::ifstream file{ "resources/games/pong2.c8", std::ios::binary };
	if (!file)
	{
		// File opening error
		std::cout << "ERROR: File could not be opened\n";
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::cout << "File Size: " << fileSize << "\n";

	if (fileSize <= (memory.size() - 512))
	{
		std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), memory.begin() + 512);
		std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::ostream_iterator<int>(std::cout, " "));
		std::cout << std::endl;
	}
	else
	{
		// File size error
	}

	std::cout << "Array section: ";
	std::copy(memory.begin() + 512, memory.begin() + 600 + 1, std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;

	return 0;
}

bool Chip8::emulateCycle()
{
	// Fetch opcode
	current_opcode = static_cast<uint16_t>(memory.at(PC)) << 8 | memory.at(PC + 1);
	std::cout << "PC: 0x" << std::hex << static_cast<int>(memory.at(PC)) << "\n";
	std::cout << "PC + 1: 0x" << std::hex << static_cast<int>(memory.at(PC + 1)) << "\n";
	std::cout << "Opcode: 0x" << std::hex << current_opcode << "\n";

	// Decode opcode
	switch (current_opcode & 0xF000)
	{
	case 0xA000:
		I = current_opcode & 0x0FFF;
		PC += 2;
		break;
	default:
		std::cout << "Instruction not implemented: 0x" << std::hex << current_opcode << "\n";
		return false;
	}

	// Execute opcode

	// Update timers
	if (delay_timer > 0)
		--delay_timer;
	if (sound_timer > 0)
	{
		if (!(--sound_timer))
			// TODO Insert actual sound
			std::cout << "BEEP!\n";
	}

	return false;
}