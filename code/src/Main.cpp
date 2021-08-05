#include <iostream>
#include <string>
#include <Windows.h>
#include "../inc/Emulator.h"

void emulatorToScreen(unsigned char* graphics, char* screen);

static const int winWidth = 64;
static const int winHeight = 32;
static const int graphicSize = winWidth * winHeight;

int main(int argc, char **argv) {
	//Graphics/sound/input: external library setup

	Emulator chip8;

	std::string romLocations("./roms/");
	std::string defaultRom("pong2");
	std::string fileExtension(".ch8");

	if(argc > 1) {
		chip8.loadGame(romLocations + argv[1] + fileExtension);
	}
	else {
		std::cout << "No specified ROM, defaulting to " << defaultRom << "." << std::endl;
		chip8.loadGame(romLocations + defaultRom + fileExtension);
	}

	char screen[graphicSize];
	for(int i = 0; i < graphicSize; i++) {
		screen[i] = '?';
	}
	/*HANDLE displayHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(displayHandle);
	DWORD dwHandle = 0;*/

	while(chip8.getRunningStatus()) {
		chip8.cycle();

		if(chip8.getDrawFlag()) {
		  chip8.setDrawFlag(false);
		  emulatorToScreen(chip8.getGraphics(),screen);
		  //WriteConsoleOutputCharacter(displayHandle, screen, winWidth * winHeight, {0,0}, &dwHandle);
		}
	}

	return 0;
}

void emulatorToScreen(unsigned char* graphics, char* screen) {
  //std::cout << graphics << std::endl;
  for(int i = 0; i < graphicSize; i++) {
    if(graphics[i] == 0) {
      screen[i] = ' ';
    }
    else if(graphics[i] == 1) {
      screen[i] = '#';
    }
	else if (graphics[i] == 2) {
		screen[i] = 'd';
	}
	else {
		screen[i] = graphics[i];
	}
  }
}
