#include <iostream>
#include <fstream>
#include "Chip8.h"
#include <thread>
#include <chrono>

#include <SDL.h>
#include <stdio.h>

using namespace std;

int main(int argc, char* args[])
{
	fstream fs;
	fs.open("res/roms/HIDDEN", fstream::in | fstream::binary);

	fs.seekg(0, fs.end);
	int len = fs.tellg();
	fs.seekg(0, fs.beg);
	
	char* romData = new char[len];
	fs.read(romData, len);
	
	Chip8 core = Chip8();
	core.initialize();
	core.loadProgram(romData, len);

	char screenBuf[64 * 32];
	for (int i = 0; i < 64 * 32; i++) {
		screenBuf[i] = 0;
	}

	while (true) {
		core.doCycle();

		if (core.drawFlag) {
			core.loadScreen(screenBuf);
		}

		system("cls");

		for (int y = 0; y < 32; y++) {
			for (int x = 0; x < 64; x++) {
				if (screenBuf[y * 64 + x]) {
					cout << "O";
				}
				else {
					cout << " ";
				}
			}
			cout << endl;
		}

		Chip8::DebugInfo info = core.dumpDebug();

		cout << hex;
		cout << "PC : " << info.pc << endl;
		cout << "OPCODE : " << info.opcode << endl;
		cout << "I : " << (int)info.i << endl << endl;

		cout << "Delay : " << info.timer_delay << endl;
		cout << "Sound : " << info.timer_sound << endl;

		for (int i = 0; i < 16; i++) {
			cout << "V[" << i << "] : " << (int)info.V[i] << " ";
			if (i % 8 == 0) {
				cout << endl;
			}
		}

		/*char in;
		cin >> in;*/

		this_thread::sleep_for(2ms);
	}

	delete[] romData;
	fs.close();
	return 0;
}