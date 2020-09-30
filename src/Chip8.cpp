#include "Chip8.h"
#include <stdlib.h>

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
	//clearScreenFlag = 1;
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
/// 8XY4
/// </summary>
void Chip8::vxAddVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	//set carry flag if overflow
	V[0xf] = (V[x] + V[y]) > 0xff;
	V[x] += V[y];
}

/// <summary>
/// 8XY5
/// </summary>
void Chip8::vxSubVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	//set carry flag if not underflow
	V[0xf] = !((V[x] - V[y]) < 0xff);
	V[x] -= V[y];
}

/// <summary>
/// 8XY6
/// </summary>
void Chip8::vxShiftR() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	//Set Vf to least significant bit of Vx
	V[0xf] = V[x] & 0x01;
	V[x] = V[x] >> 1;
}

/// <summary>
/// 8XY7
/// </summary>
void Chip8::vxToVySubVx() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	//set carry flag if not underflow
	V[0xf] = !((V[y] - V[x]) < 0xff);
	V[x] = V[y] - V[x];
}

/// <summary>
/// 8XYE
/// </summary>
void Chip8::vxShiftL() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	//Set Vf to most significant bit of Vx
	V[0xf] = V[x] & 0x80;
	V[x] = V[x] << 1;
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
/// ANNN
/// </summary>
void Chip8::iToNNN() {
	unsigned short nnn = (opcode & 0x0fff);
	I = nnn;
}

/// <summary>
/// BNNN
/// </summary>
void Chip8::jmpToNNNAddV0() {
	unsigned short nnn = (opcode & 0x0fff);
	pc = nnn + V[0];
}

/// <summary>
/// CXNN
/// </summary>
void Chip8::randAndNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char nn = (opcode & 0x00ff);
	V[x] = (rand() % 0x100) & nn;
}

/// <summary>
/// DXYN
/// </summary>
void Chip8::draw() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	unsigned char n = (opcode & 0x000f);
	for (int row = 0; row <= n; row++) {
		for (int col = 0; col < 8; col++) {
			unsigned char bit = (memory[I + (row)] >> (7 - col)) & 0x01;
			unsigned int pos = 64 * (y + row) + (x + col);
			V[0xf] = graphic[pos] & bit;
			graphic[pos] = (graphic[pos]^bit);
		}
	}

	drawFlag = 1;
}

/// <summary>
/// EX9E
/// </summary>
void Chip8::ifKeyEqVx() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	if (key[V[x]] != 0) {
		pc += 2;
	}
}

/// <summary>
/// EXA1
/// </summary>
void Chip8::ifKeyNotEqVx() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	if (key[V[x]] == 0) {
		pc += 2;
	}
}

/// <summary>
/// FX07
/// </summary>
void Chip8::getDelay() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	V[x] = delay_timer;
}

/// <summary>
/// FX0A
/// </summary>
void Chip8::waitKey() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	for (unsigned char i = 0; i < 16; i++) {
		if (key[i] != 0) {
			V[x] = i;
			interruptFlag = 0;
			return;
		}
	}
	interruptFlag = 1;
}

/// <summary>
/// FX15
/// </summary>
void Chip8::setDelay() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	delay_timer = V[x];
}

/// <summary>
/// FX18
/// </summary>
void Chip8::setSoundTimer() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	sound_timer = V[x];
}

/// <summary>
/// FX1E
/// </summary>
void Chip8::iAddVx() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	I += V[x];
}

/// <summary>
/// FX29
/// </summary>
void Chip8::iToSprAdd() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	//each character requires 5 bytes of data. stored from 0x050
	I = 0x050 + 5 * V[x];
}


/// <summary>
/// FX33
/// </summary>
void Chip8::setBCD() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned short vx = V[x];
	memory[I] = vx / 100;
	vx -= (vx/100) * 100;
	memory[I + 1] = vx / 10;
	vx -= (vx / 10) * 10;
	memory[I + 2] = vx;
}

/// <summary>
/// FX55
/// </summary>
void Chip8::regDump() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	for (int i = 0; i <= x; i++) {
		memory[I + i] = V[i];
	}
}

/// <summary>
/// FX65
/// </summary>
void Chip8::regLoad() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	for (int i = 0; i <= x; i++) {
		V[i] = memory[I + i];
	}
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

	if (!interruptFlag) {
		pc += 2; //Increase program counter by 2 since each opcode is 2 bytes long
	}
}