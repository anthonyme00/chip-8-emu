#include "Chip8.h"
#include <stdlib.h>
#include <iostream>

#define FONTSET_OFFSET 0x050
#define PROGRAM_OFFSET 0x200

/// <summary>
/// 0x0NNN Machine code subroutine
/// Shouldn't be used
/// </summary>
void Chip8::call() {
	//Do nothing. This is only for old machines.
}

/// <summary>
/// 00E0
/// Clears the display
/// </summary>
void Chip8::disp_clear() {
	for (int i = 0; i < 64 * 32; i++) {
		graphic[i] = 0;
	}
	drawFlag = 1;
}

/// <summary>
/// 00EE
///	Return from a subroutine by getting the previous
/// Address from the stack
/// </summary>
void Chip8::ret() {
	sp--;
	pc = stack[sp];
}

/// <summary>
/// 1NNN
/// Shift the program counter to a different memory location
/// </summary>
void Chip8::go_to() {
	pc = opcode & 0x0FFF; //jump to NNN
}

/// <summary>
/// 2NNN
/// Shift the program counter to a different memory location
/// while storing the current memory location so we can return
/// to it later
/// </summary>
void Chip8::subroutine() {
	stack[sp] = pc; //set current stack to program counter
	sp++; //increment stack pointer by 1
	pc = opcode & 0x0FFF; //set pc to NNN
}

/// <summary>
/// 3XNN
/// If register V[x] is equal to NN, then skip the following
/// OPCODE
/// </summary>
void Chip8::ifVxNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	if (V[x] == (opcode & 0x00ff)) {
		pc += 2;
		return;
	}
}

/// <summary>
/// 4XNN
/// If register V[x] is NOT equal to NN, then skip the following
/// OPCODE
/// </summary>
void Chip8::ifVxNotNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	if (V[x] != (opcode & 0x00ff)) {
		pc += 2;
		return;
	}
}

/// <summary>
/// 5XY0
/// If register V[x] is equal to register V[y] then skip the following
/// OPCODE
/// </summary>
void Chip8::ifVxVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	if (V[x] == V[y]) {
		pc += 2;
		return;
	}
}

/// <summary>
/// 6XNN
/// Set register V[x] to NN
/// </summary>
void Chip8::vxToNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char nn = (opcode & 0x00ff);
	V[x] = nn;
}

/// <summary>
/// 7XNN
/// Add NN to register V[x]
/// </summary>
void Chip8::vxAddNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char nn = (opcode & 0x00ff);
	V[x] += nn;
}

/// <summary>
/// 8XY0
/// Set register V[x] to V[y]
/// </summary>
void Chip8::vxToVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	V[x] = V[y];
}

/// <summary>
/// 8XY1
/// Do OR bit op on V[x] with V[y]
/// </summary>
void Chip8::vxOrVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	V[x] |= V[y];
}

/// <summary>
/// 8XY2
/// Do AND bit op on V[x] with V[y]
/// </summary>
void Chip8::vxAndVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	V[x] &= V[y];
}

/// <summary>
/// 8XY3
/// Do XOR bit op on V[x] with V[y]
/// </summary>
void Chip8::vxXorVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	V[x] ^= V[y];
}

/// <summary>
/// 8XY4
/// Add V[y] to V[x]
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
/// Subtract V[y] from V[x]
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
/// Bit shift V[x] to the right while storing the least
/// significant bit to V[f]
/// </summary>
void Chip8::vxShiftR() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	//Set Vf to least significant bit of Vx
	V[0xf] = V[x] & 0x01;
	V[x] = V[x] >> 1;
}

/// <summary>
/// 8XY7
/// Set V[x] to V[y] subtracted by V[y]
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
/// Bit shift V[x] to the left while storing the most
/// significant bit to V[f]
/// </summary>
void Chip8::vxShiftL() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	//Set Vf to most significant bit of Vx
	V[0xf] = V[x] & 0x80;
	V[x] = V[x] << 1;
}

/// <summary>
/// 9XY0
/// If V[x] is NOT equals to V[y] then skip the following
/// OPCODE
/// </summary>
void Chip8::ifVxNotVy() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	if (V[x] != V[y]) {
		pc += 2;
		return;
	}
}

/// <summary>
/// ANNN
/// Sets I memory pointer register to NNN
/// </summary>
void Chip8::iToNNN() {
	unsigned short nnn = (opcode & 0x0fff);
	I = nnn;
}

/// <summary>
/// BNNN
/// Sets PC to NNN added with V[0]
/// </summary>
void Chip8::jmpToNNNAddV0() {
	unsigned short nnn = (opcode & 0x0fff);
	pc = nnn + V[0];
}

/// <summary>
/// CXNN
/// Sets V[x] to a random number with NN as a bit mask
/// applied to it
/// </summary>
void Chip8::randAndNN() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char nn = (opcode & 0x00ff);
	V[x] = (rand() % 0x100) & nn;
}

/// <summary>
/// DXYN
/// Draw a sprite of height N at position (V[x], V[y]),
/// with the sprite location in memory pointed to by I
/// </summary>
void Chip8::draw() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	unsigned char y = (opcode & 0x00f0) >> 4;
	unsigned char n = (opcode & 0x000f);
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < 8; col++) {
			unsigned char bit = (memory[I + (row)] >> (7 - col)) & 0x01;
			unsigned int pos = 64 * (V[y] + row) + (V[x] + col);
			V[0xf] = graphic[pos] & bit;
			graphic[pos] = (graphic[pos]^bit);
		}
	}

	drawFlag = 1;
}

/// <summary>
/// EX9E
/// If a key is pressed that is equals to V[x] then
/// skip the following OPCODE
/// </summary>
void Chip8::ifKeyEqVx() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	if (key[V[x]] != 0) {
		pc += 2;
		return;
	}
}

/// <summary>
/// EXA1
/// If a key is pressed that is NOT equals to V[x] then
/// skip the following OPCODE
/// </summary>
void Chip8::ifKeyNotEqVx() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	if (key[V[x]] == 0) {
		pc += 2;
		return;
	}
}

/// <summary>
/// FX07
/// Sets V[x] to the current value of delay_timer
/// </summary>
void Chip8::getDelay() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	V[x] = delay_timer;
}

/// <summary>
/// FX0A
/// Wait for a key to be pressed and then store
/// the keycode in V[x]. This is a blocking operation
/// which means no further OPCODE will be run when this is running
/// </summary>
void Chip8::waitKey() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	for (unsigned char i = 0; i < 16; i++) {
		if (key[i] != 0) {
			V[x] = i;
			return;
		}
	}
	//simulate blocking
	//TODO: DO PROPER BLOCKING
	pc -= 2;
}

/// <summary>
/// FX15
/// Set delay_timer to V[x]
/// </summary>
void Chip8::setDelay() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	delay_timer = V[x];
}

/// <summary>
/// FX18
/// Set sound_timer to V[x]
/// </summary>
void Chip8::setSoundTimer() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	sound_timer = V[x];
}

/// <summary>
/// FX1E
/// Adds V[x] to I
/// </summary>
void Chip8::iAddVx() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	I += V[x];
}

/// <summary>
/// FX29
/// Sets I to the location of character graphic V[x] in memory
/// </summary>
void Chip8::iToSprAdd() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	//each character requires 5 bytes of data. stored from 0x050
	I = FONTSET_OFFSET + 5 * V[x];
}

/// <summary>
/// FX33
/// Stores the BCD representation of V[x] in memory.
/// Uses 3 bytes of memory, starting from I to I+2
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
/// Dumps the content of our memory from I to x.
/// Stores it in V[0] to V[x]
/// </summary>
void Chip8::regDump() {
	unsigned char x = (opcode & 0x0f00) >> 8;
	for (int i = 0; i <= x; i++) {
		memory[I + i] = V[i];
	}
}

/// <summary>
/// FX65
/// Loads the content of V[0] to V[x] to memory.
/// Stores it in I to I+x
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
	pc = PROGRAM_OFFSET; //Program resides in memory address starting from 0x200
	opcode = 0;
	I = 0;
	sp = 0;

	delay_timer = 0;
	sound_timer = 0;
	sleepTimer = 0;

	//Clear general register, stack, and key
	for (int i = 0; i < 16; i++) {
		V[i] = 0;
		stack[i] = 0;
		key[i] = 0;
	}

	drawFlag = 0;
	
	//Clear screen
	for (int i = 0; i < 64 * 32; i++) {
		graphic[i] = 0;
	}

	//Clear memory
	for (int i = 0; i < 4096; i++) {
		memory[i] = 0;
	}

	/// <summary>
	/// This is the default Chip8 fontset. acquired from
	///	http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
	/// </summary>
	unsigned char chip8_fontset[80] =
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
	//Fill memory FONTSET_OFFSET with fontset
	for (int i = 0; i < 16; i++) {
		for (int x = 0; x < 5; x++) {
			memory[FONTSET_OFFSET + (i * 5) + x] = chip8_fontset[(i * 5) + x];
		}
	}
}

/// <summary>
/// Fetches next OPCODE
/// </summary>
/// <returns>our opcode</returns>
unsigned short Chip8::fetch() {
	unsigned short op = memory[pc] << 8 | memory[pc + 1];
	pc += 2;
	return op;
}


void Chip8::doCycle() {
	//every opcode is 2 bytes long. stored in big endian
	opcode = fetch();

	//reset draw flag
	drawFlag = 0;

	//this is a very basic timer implementation.
	//since CHIP8 has a refresh rate of 500hz, Roughly
	//1/60th of a second passes every 8 clock pulse.
	//in reality, this should asynchronous.
	//TODO: DO PROPER TIMING
	sleepTimer++;
	sleepTimer %= 8;

	if (sleepTimer == 0) {
		if (delay_timer > 0) delay_timer--;
		if (sound_timer > 0) sound_timer--;
	}

	//channel the opcode to correct operation
	switch ((opcode & 0xf000) >> 12) {
	case 0x0:
		switch ((opcode & 0x00ff)) {
		case 0x00ee:
			ret();
			break;
		case 0x00e0:
			disp_clear();
			break;
		default:
			call();
			break;
		}
		break;
	case 0x1:
		go_to();
		break;
	case 0x2:
		subroutine();
		break;
	case 0x3:
		ifVxNN();
		break;
	case 0x4:
		ifVxNotNN();
		break;
	case 0x5:
		ifVxVy();
		break;
	case 0x6:
		vxToNN();
		break;
	case 0x7:
		vxAddNN();
		break;
	case 0x8:
		switch (opcode & 0x000f) {
		case 0x0:
			vxToVy();
			break;
		case 0x1:
			vxOrVy();
			break;
		case 0x2:
			vxAndVy();
			break;
		case 0x3:
			vxXorVy();
			break;
		case 0x4:
			vxAddVy();
			break;
		case 0x5:
			vxSubVy();
			break;
		case 0x6:
			vxShiftR();
			break;
		case 0x7:
			vxToVySubVx();
			break;
		case 0xe:
			vxShiftL();
			break;
		}
		break;
	case 0x9:
		ifVxNotVy();
		break;
	case 0xa:
		iToNNN();
		break;
	case 0xb:
		jmpToNNNAddV0();
		break;
	case 0xc:
		randAndNN();
		break;
	case 0xd:
		draw();
		break;
	case 0xe:
		switch (opcode & 0x00ff) {
		case 0x9e:
			ifKeyEqVx();
			break;
		case 0xa1:
			ifKeyNotEqVx();
			break;
		}
		break;
	case 0xf:
		switch (opcode & 0x00ff) {
		case 0x07:
			getDelay();
			break;
		case 0x0a:
			waitKey();
			break;
		case 0x15:
			setDelay();
			break;
		case 0x18:
			setSoundTimer();
			break;
		case 0x1e:
			iAddVx();
			break;
		case 0x29:
			iToSprAdd();
			break;
		case 0x33:
			setBCD();
			break;
		case 0x55:
			regDump();
			break;
		case 0x65:
			regLoad();
			break;
		}
		break;
	}
}

/// <summary>
/// Load rom to our Chip-8 Machine
/// </summary>
/// <param name="data">data to load</param>
/// <param name="len">size of data in bytes</param>
void Chip8::loadProgram(char* data, int len) {
	for (int i = 0; i < len; i++) {
		memory[PROGRAM_OFFSET + i] = data[i];
	}
}

/// <summary>
/// Fill a char buffer with our screen information
/// </summary>
/// <param name="screenBuf">buffer to fill</param>
void Chip8::loadScreen(char* screenBuf) {
	for (int i = 0; i < 64 * 32; i++) {
		screenBuf[i] = graphic[i];
	}
}