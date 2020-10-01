#include <iostream>
#include <fstream>
#include "Chip8.h"
#include <thread>
#include <chrono>

using namespace std;

int main()
{
	fstream fs;
	fs.open("../res/roms/PONG", fstream::in | fstream::binary);

	fs.seekg(0, fs.end);
	int len = fs.tellg();
	fs.seekg(0, fs.beg);

	char* romData = new char[len];
	fs.read(romData, len);
	
	Chip8 core = Chip8();
	core.initialize();
	core.loadProgram(romData, len);

	char screenBuf[64 * 32];
	while (true) {
		core.doCycle();
		if (core.drawFlag) {
			system("cls");
			core.loadScreen(screenBuf);
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
		}
		std::this_thread::sleep_for(2ms);
	}

	delete[] romData;
	fs.close();
	return 0;
}