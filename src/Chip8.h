#pragma once
/// <summary>
/// Chip 8 Implementation
/// Based on:
/// http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
/// https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
/// </summary>
class Chip8
{
private:
	unsigned short opcode;
	/*
	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM
	*/
	unsigned char memory[4096]; //Allocate 4K Memory
	unsigned char V[16]; //General purpose registers
	unsigned short I; //Index register
	unsigned short pc; //Program counter
	unsigned char graphic[64 * 32]; //Monochrome screen. Only need 8*4 bytes. is 64*32 for simplicity

	/// <summary>
	/// Both of these count down to 0. These are refreshed at a frequency of 60hz
	/// for sound_timer in particular, it sounds a buzzer when it reaches 0
	/// </summary>
	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned short stack[16]; //Store current memory location whenever a subroutine is called
	unsigned short sp; //Stack pointer

	unsigned char key[16]; //Track current position of key
	unsigned char clearScreenFlag;

	//call
	void call();
	//display
	void disp_clear();
	//flow
	void ret();
	void go_to();
	void subroutine();
	//cond
	void ifVxNN();
	void ifVxNotNN();
	void ifVxVy();
	void ifVxNotVy();
	//const
	void vxToNN();
	void vxAddNN();
	//assign
	void vxToVy();
	//bitOp
	void vxOrVy();
	void vxAndVy();
	void vxXorVy();
public:
	void initialize();
	void doCycle();
};