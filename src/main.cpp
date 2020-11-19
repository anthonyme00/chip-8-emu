#include "Chip8.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <stdio.h>

#include <SDL.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

#include <GL/gl3w.h>

#include <nfd.h>

#define WINDOW_RES_X 640
#define WINDOW_RES_Y 480

SDL_Window* window = NULL;
SDL_GLContext gl_context = NULL;

#define GLSL_VER "#version 130"
const int gl_major_ver = 3;
const int gl_minor_ver = 0;

Chip8 core;
bool rom_loaded = false;

bool init() 
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		return false;
	}

	//Use OpenGL 3.0 + GLSL 1.30
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);
	//Do not use deprecated functions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


	window = SDL_CreateWindow("Chip-8 Emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_RES_X, WINDOW_RES_Y, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	//Disable opengl because this makes emulation slow
	SDL_GL_SetSwapInterval(0);

	if (gl3wInit() != 0)
	{
		return false;
	}

	return true;
}

void init_imgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(GLSL_VER);
}

void draw_imgui() 
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open Rom")) {
				nfdchar_t *path = NULL;
				nfdresult_t result = NFD_OpenDialog(NULL, NULL, &path);

				if (result == NFD_OKAY) {
					core.initialize();

					std::fstream fs;
					fs.open(path, std::fstream::in | std::fstream::binary);

					fs.seekg(0, fs.end);
					int len = fs.tellg();
					fs.seekg(0, fs.beg);

					char* romData = new char[len];
					fs.read(romData, len);

					core.loadProgram(romData, len);
					rom_loaded = true;

					delete[] romData;
					fs.close();
				}
			}
			ImGui::EndMenu();
		}		
		ImGui::EndMainMenuBar();
	}
	/*
	ImGui::Begin("A Window", 0, ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize);
	ImGui::Text("Hello World");
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2(0, 100));
	ImGui::Begin("A Window 2", 0, ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Hello World 2");
	ImGui::End();
	*/
	
}

void cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


int main(int argc, char* args[])
{
	/*
	std::fstream fs;
	fs.open("res/roms/INVADERS", std::fstream::in | std::fstream::binary);

	fs.seekg(0, fs.end);
	int len = fs.tellg();
	fs.seekg(0, fs.beg);
	
	char* romData = new char[len];
	fs.read(romData, len);
	*/

	core = Chip8();
	core.initialize();
	//core.loadProgram(romData, len);

	init();

	init_imgui();

	SDL_Event e;

	unsigned char screenBuf[64 * 32 * 3];
	for (int i = 0; i < 64 * 32 * 3; i++) {
		screenBuf[i] = 0;
	}

	bool exit = false;

	unsigned char keybuf[16] = { 0, 0, 0, 0,
								 0, 0, 0, 0,
								 0, 0, 0, 0,
								 0, 0, 0, 0 };


	GLuint chip_8_window;
	glGenTextures(1, &chip_8_window);
	glBindTexture(GL_TEXTURE_2D, chip_8_window);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, screenBuf);

	while (!exit) {
		glViewport(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		if(rom_loaded)
			core.doCycle();

		while (SDL_PollEvent(&e) != 0) {
			ImGui_ImplSDL2_ProcessEvent(&e);

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

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
		draw_imgui();

		if (core.drawFlag) {
			core.loadScreen(screenBuf);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glBindTexture(GL_TEXTURE_2D, chip_8_window);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 32, GL_RGB, GL_UNSIGNED_BYTE, screenBuf);
		}
		
		ImGui::SetNextWindowSize(ImVec2(530, 300));
		ImGui::SetNextWindowPos(ImVec2(0, 25));
		ImGui::Begin("Chip-8", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Image((void*)chip_8_window, ImVec2(512, 256));
		ImGui::End();

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);

		/*
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
		*/

		std::this_thread::sleep_for(std::chrono::nanoseconds(2500));
	}

	SDL_DestroyWindow(window);

	return 0;
}