# CHIP-8-EMU
A simple chip-8 emulator.
You can get roms from [here](http://www.pong-story.com/chip8/).

![v0.1a](https://github.com/anthonyme00/github-markdown-resource/blob/main/chip-8-emu/chip-8-sdl-1.png "Release v0.1a")

This project was created to learn about how computer works at a very basic level.

Dependencies:
 - [SDL2](https://www.libsdl.org/)
 - [NFD](https://github.com/mlabbe/nativefiledialog)
 - [Dear ImGui](https://github.com/ocornut/imgui)

### Controls:
```
+-------+                 +-------+
|1|2|3|4|                 |1|2|3|C|
+-------+                 +-------+
|Q|W|E|R|                 |4|5|6|D|
+-------+ (Keyboard) =>   +-------+ (Chip-8 Keypad)
|A|S|D|F|                 |7|8|9|E|
+-------+                 +-------+
|Z|X|C|R|                 |A|0|B|F|
+-------+                 +-------+
```

## Building
Prerequisites:
- Visual Studio 2019
- Python
- SDL 2 Development Lib (see below)

## Building
Prerequisites:
- Visual Studio 2019
- Python
- SDL 2 Development Lib (see below)

Clone this repository

```
git clone https://github.com/anthonyme00/chip-8-emu/
```

Initialize and update the submodules

```
git submodule init
git submodule update
```

Download [SDL2 Development Library](https://www.libsdl.org/download-2.0.php) for Visual C++. Extract the folder inside, rename it to SDL2 and put it in
```
libs\SDL2
```

After which the folder structure should look like
```
libs\SDL2
libs\SDL2\docs
libs\SDL2\include
libs\SDL2\lib
...
```

Go to libs\gl3w and run gl3w_gen.py
```
python gl3w.py
```

Open the `chip-8.sln` file and build.

## In Action
***
PONG

![pong](https://github.com/anthonyme00/github-markdown-resource/blob/main/chip-8-emu/chip-8-emu-pong.gif "PONG!")

#### Outdated Console Version
PONG

![pong](https://github.com/anthonyme00/github-markdown-resource/blob/main/chip-8-emu/chip-8-emu-pong.png "PONG!")

UFO

![ufo](https://github.com/anthonyme00/github-markdown-resource/blob/main/chip-8-emu/chip-8-emu-ufo.png "UFO!")
