#pragma once
/// <summary>
/// Chip 8 Implementation
/// ===================================================================================
/// Based on:
/// http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
/// https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
/// ===================================================================================
/// For info on the private functions, see Chip8.cpp
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
	/// for sound_timer in particular, it sounds a buzzer when it is not 0
	/// </summary>
	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned short stack[16]; //Store current memory location whenever a subroutine is called
	unsigned short sp; //Stack pointer

	unsigned char key[16]; //Track current position of key
	unsigned short sleepTimer;

	unsigned short fetch();

	//call
	void call();
	//display
	void disp_clear();
	void draw();
	//flow
	void ret();
	void go_to();
	void subroutine();

	void jmpToNNNAddV0();
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

	void vxShiftR();
	void vxShiftL();
	//math
	void vxAddVy();
	void vxSubVy();
	void vxToVySubVx();
	//mem
	void iToNNN();

	void iAddVx();
	void iToSprAdd();

	void regDump();
	void regLoad();
	//rand
	void randAndNN();
	//keyOp
	void ifKeyEqVx();
	void ifKeyNotEqVx();
	void waitKey();
	//timer
	void getDelay();
	void setDelay();
	//sound
	void setSoundTimer();
	//bcd
	void setBCD();
public:
	void initialize();
	void loadProgram(char* data, int len);
	void loadScreen(char* screenBuf);
	void loadKey(unsigned char* keys);
	void doCycle();

	/// <summary>
	/// Basic debugging info for our CHIP-8 machine
	/// </summary>
	struct DebugInfo {
		unsigned short pc = 0;
		unsigned short opcode = 0;
		unsigned char V[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		unsigned char i = 0;
		unsigned short timer_delay = 0;
		unsigned short timer_sound = 0;

		DebugInfo(unsigned short pc, unsigned short opcode, unsigned char* V, unsigned char i, unsigned timer_delay, unsigned timer_sound) {
			this->pc = pc;
			this->opcode = opcode;
			for (int i = 0; i < 16; i++) {
				this->V[i] = V[i];
			}
			this->i = i;

			this->timer_delay = timer_delay;
			this->timer_sound = timer_sound;
		}
	};

	DebugInfo dumpDebug() {
		return DebugInfo(pc, opcode, V, I, delay_timer, sound_timer);
	}

	unsigned char drawFlag;
};