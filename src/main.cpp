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
	fs.open("res/roms/PONG", fstream::in | fstream::binary);

	fs.seekg(0, fs.end);
	int len = fs.tellg();
	fs.seekg(0, fs.beg);
	
	char* romData = new char[len];
	fs.read(romData, len);
	
	Chip8 core = Chip8();
	core.initialize();
	core.loadProgram(romData, len);

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("CHIP-8EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 256, SDL_WINDOW_SHOWN);
	screenSurface = SDL_GetWindowSurface(window);
	unsigned char* pixels = (unsigned char*)screenSurface->pixels;
	SDL_PixelFormat* fmt = screenSurface->format;

	SDL_Event e;

	char screenBuf[64 * 32];
	for (int i = 0; i < 64 * 32; i++) {
		screenBuf[i] = 0;
	}

	bool exit = false;

	unsigned char keybuf[16] = { 0, 0, 0, 0,
								 0, 0, 0, 0,
								 0, 0, 0, 0,
								 0, 0, 0, 0 };

	while (!exit) {
		core.doCycle();

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				exit = true;
			}
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
				unsigned char input = 0;
				if (e.type == SDL_KEYDOWN) input = 1;
				switch (e.key.keysym.sym)
				{
				case SDLK_1:
					keybuf[0x1] = input;
					break;
				case SDLK_2:
					keybuf[0x2] = input;
					break;
				case SDLK_3:
					keybuf[0x3] = input;
					break;
				case SDLK_4:
					keybuf[0xc] = input;
					break;
				case SDLK_q:
					keybuf[0x4] = input;
					break;
				case SDLK_w:
					keybuf[0x5] = input;
					break;
				case SDLK_e:
					keybuf[0x6] = input;
					break;
				case SDLK_r:
					keybuf[0xd] = input;
					break;
				case SDLK_a:
					keybuf[0x7] = input;
					break;
				case SDLK_s:
					keybuf[0x8] = input;
					break;
				case SDLK_d:
					keybuf[0x9] = input;
					break;
				case SDLK_f:
					keybuf[0xe] = input;
					break;
				case SDLK_z:
					keybuf[0xa] = input;
					break;
				case SDLK_x:
					keybuf[0x0] = input;
					break;
				case SDLK_c:
					keybuf[0xb] = input;
					break;
				case SDLK_v:
					keybuf[0xf] = input;
					break;
				}
			}
			core.loadKey(keybuf);
		}

		if (core.drawFlag) {
			core.loadScreen(screenBuf);

			SDL_LockSurface(screenSurface);
			for (int x = 0; x < 512; x++) {
				for (int y = 0; y < 256; y++) {
					pixels[x * fmt->BytesPerPixel + y * screenSurface->pitch] = 255 * screenBuf[(y / 8) * 64 + (x / 8)];
					pixels[x * fmt->BytesPerPixel + y * screenSurface->pitch + 1] = 255 * screenBuf[(y / 8) * 64 + (x / 8)];
					pixels[x * fmt->BytesPerPixel + y * screenSurface->pitch + 2] = 255 * screenBuf[(y / 8) * 64 + (x / 8)];
					pixels[x * fmt->BytesPerPixel + y * screenSurface->pitch + 3] = 255 * screenBuf[(y / 8) * 64 + (x / 8)];
				}
			}
			SDL_UnlockSurface(screenSurface);

			SDL_UpdateWindowSurface(window);
		}

		/*
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
			if (i % 8 == 0) {
				cout << endl;
			}
			cout << "V[" << i << "] : " << (int)info.V[i] << "\t";
		}
		*/

		this_thread::sleep_for(5000ns);
	}

	SDL_FreeSurface(screenSurface);
	SDL_DestroyWindow(window);

	delete[] romData;
	fs.close();
	return 0;
}