#include "Chip8.h"

/// <summary>
/// 0x0NNN Machine code subroutine
/// Presumably shouldn't be the exact same as instruction
/// 2NNN.
/// TODO: Research further
/// </summary>
void Chip8::call() {
	stack[sp] = pc; //set current stack to program counter
	sp++; //increment stack pointer by 1
	pc = opcode & 0x0FFF; //set pc to NNN
}

/// <summary>
/// 00E0
/// </summary>
void Chip8::disp_clear() {
	clearScreenFlag = 1;
}

/// <summary>
/// 00EE
/// </summary>
void Chip8::ret() {
	sp--;
	pc = stack[sp];
}

/// <summary>
/// 1NNN
/// </summary>
void Chip8::go_to() {
	pc = opcode & 0x0FFF; //jump to NNN
}

/// <summary>
/// 2NNN
/// </summary>
void Chip8::subroutine() {
	stack[sp] = pc; //set current stack to program counter
	sp++; //increment stack pointer by 1
	pc = opcode & 0x0FFF; //set pc to NNN
}

/// <summary>
/// 3XNN
/// </summary>
void Chip8::ifVxNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	if (V[x] == (opcode & 0x00ff)) {
		pc += 2;
	}
}

/// <summary>
/// 4XNN
/// </summary>
void Chip8::ifVxNotNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	if (V[x] != (opcode & 0x00ff)) {
		pc += 2;
	}
}

/// <summary>
/// 5XY0
/// </summary>
void Chip8::ifVxVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	if (V[x] == V[y]) {
		pc += 2;
	}
}

/// <summary>
/// 9XY0
/// </summary>
void Chip8::ifVxNotVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	if (V[x] != V[y]) {
		pc += 2;
	}
}

/// <summary>
/// 6XNN
/// </summary>
void Chip8::vxToNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char nn = (opcode & 0x00ff);
	V[x] = nn;
}

/// <summary>
/// 7XNN
/// </summary>
void Chip8::vxAddNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char nn = (opcode & 0x00ff);
	V[x] += nn;
}

/// <summary>
/// 8XY0
/// </summary>
void Chip8::vxToVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	V[x] = V[y];
}

/// <summary>
/// 8XY1
/// </summary>
void Chip8::vxOrVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	V[x] |= V[y];
}

/// <summary>
/// 8XY2
/// </summary>
void Chip8::vxAndVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	V[x] &= V[y];
}

/// <summary>
/// 8XY3
/// </summary>
void Chip8::vxXorVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	V[x] ^= V[y];
}

/// <summary>
/// TODO:
/// Initialize all to it's proper value
/// </summary>
void Chip8::initialize() {
	pc = 0x200; //program resides in memory address starting from 0x200
	opcode = 0;
	I = 0;
	sp = 0;
}

void Chip8::doCycle() {
	//every opcode is 2 bytes long. stored in big endian
	unsigned short opcode = memory[pc] << 8 | memory[pc+1];

	pc += 2; //Increase program counter by 2 since each opcode is 2 bytes long
}