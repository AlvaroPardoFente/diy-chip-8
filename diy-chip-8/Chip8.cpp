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
	initJumpTables();

	PC = 0x200;
	opcode = 0;
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

const std::bitset<64 * 32>& Chip8::getGfx()
{
	return gfx;
}

bool Chip8::emulateCycle()
{
	// Fetch opcode
	try
	{
		opcode = static_cast<uint16_t>(memory.at(PC)) << 8 | memory.at(PC + 1);
	}
	catch (const std::out_of_range&)
	{
		std::cerr << "ERROR: PC is out of memory bounds\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "PC: 0x" << std::hex << static_cast<int>(memory.at(PC)) << "\n";
	std::cout << "PC + 1: 0x" << std::hex << static_cast<int>(memory.at(PC + 1)) << "\n";
	std::cout << "Opcode: 0x" << std::hex << opcode << "\n";

	// Decode opcode
	/*switch (opcode & 0xF000)
	{
	case 0xA000:
		I = opcode & 0x0FFF;
		PC += 2;
		break;
	default:
		std::cout << "Instruction not implemented: 0x" << std::hex << opcode << "\n";
		return false;
	}*/
	firstWordInstructions[(opcode & 0xF000) >> 12]();

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

	return true;
}

//
//
// ********************************** INSTRUCTION EXECUTIONS **********************************
//
//

void Chip8::initJumpTables()
{
	firstWordInstructions.fill(std::bind(&Chip8::cpuNULL, this));
	firstWordInstructions[0x0] = std::bind(&Chip8::i0x0, this);
	firstWordInstructions[0x1] = std::bind(&Chip8::i0x1NNN, this);
	firstWordInstructions[0x2] = std::bind(&Chip8::i0x2NNN, this);
	firstWordInstructions[0x3] = std::bind(&Chip8::i0x3XKK, this);
	firstWordInstructions[0x4] = std::bind(&Chip8::i0x4XKK, this);
	firstWordInstructions[0x5] = std::bind(&Chip8::i0x5XY0, this);
	firstWordInstructions[0x6] = std::bind(&Chip8::i0x6XKK, this);
	firstWordInstructions[0x7] = std::bind(&Chip8::i0xANNN, this);
	firstWordInstructions[0x8] = std::bind(&Chip8::i0xANNN, this);
	firstWordInstructions[0xA] = std::bind(&Chip8::i0xANNN, this);
}

// Default
void Chip8::cpuNULL()
{
	std::cout << "Instruction not implemented: [0x" << std::hex << opcode << "]\n";
	PC += 2;
}

// ************************** First level instructions **************************

// 0x0 handler
void Chip8::i0x0()
{
	switch (opcode)
	{
	case 0x00E0:
		i0x00E0();
		break;
	case 0x00EE:
		i0x00EE();
	default:
		i0x0NNN();
		break;
	}
}

// Jump to location nnn
void Chip8::i0x1NNN()
{
	PC = opcode & 0x0FFF;
}

// Call subroutine at nnn
void Chip8::i0x2NNN()
{
	stack.at(stack_pointer) = PC;
	++stack_pointer;
	PC = opcode & 0x0FFF;
}

// Skip next instruction if Vx = kk
void Chip8::i0x3XKK()
{
	if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		PC += 2;
	PC += 2;
}

// Skip next instruction if Vx != kk
void Chip8::i0x4XKK()
{
	if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		PC += 2;
	PC += 2;
}

// Skip next instruction if Vx = Vy
void Chip8::i0x5XY0()
{
	if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
		PC += 2;
	PC += 2;
}

// Set Vx = kk
void Chip8::i0x6XKK()
{
	V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
	PC += 2;
}

// Set Vx = Vx + kk
void Chip8::i0x7XKK()
{
	V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
	PC += 2;
}

// 0x8 handler
void Chip8::i0x8()
{
	switch (opcode & 0x000F)
	{
	case 0x0:
	case 0x1:
	case 0x2:
	case 0x3:
	case 0x4:
		i0x8XY4();
		break;
	case 0x5:
	case 0x6:
	case 0x7:
	case 0xE:
	default:
		cpuNULL();
		break;
	}
}

// Set I = nnn
void Chip8::i0xANNN()
{
	I = opcode & 0x0FFF;
	PC += 2;
}

// ************************** 0x0 instructions **************************

// Jump to a machine code routine at nnn
void Chip8::i0x0NNN()
{
	cpuNULL();	// Not implemented
}

// Clear the display
void Chip8::i0x00E0()
{
	gfx.reset();
	drawFlag = true;
	PC += 2;
}

// Return from a subroutine
void Chip8::i0x00EE()
{
	--stack_pointer;
	PC = stack.at(stack_pointer);
	PC += 2;
}

// 0x8 instructions

// Set Vx = Vy
void Chip8::i0x8XY0()
{
	V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
	PC += 2;
}

// Set Vx = Vx OR Vy
void Chip8::i0x8XY1()
{
	V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
	PC += 2;
}

// Set Vx = Vx AND Vy
void Chip8::i0x8XY2()
{
	V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
	PC += 2;
}

// Set Vx = Vx XOR Vy
void Chip8::i0x8XY3()
{
	V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
	PC += 2;
}

// Set Vx = Vx + Vy, set VF = carry
void Chip8::i0x8XY4()
{
	if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
		V[0xF] = 1;
	else
		V[0xF] = 0;
	V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
	PC += 2;
}

// Set Vx = Vx - Vy, set VF = NOT borrow
void Chip8::i0x8XY5()
{
	if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
		V[0xF] = 1;
	else
		V[0xF] = 0;
	V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
	PC += 2;
}