#include <iostream>
#include <string>
#include "../inc/Emulator.h"

int main(int argc, char **argv) {
	//Graphics/sound/input: external library setup

	Emulator chip8;

	std::string romLocations("./roms/");
	std::string defaultRom("pong");
	std::string fileExtension(".ch8");

	if(argc > 1) {
		chip8.loadGame(romLocations + argv[1] + fileExtension);
	}
	else {
		std::cout << "No specified ROM, defaulting to " << defaultRom << "." << std::endl;
		chip8.loadGame(romLocations + defaultRom + fileExtension);
	}

	while(chip8.getRunningStatus()) {

	}

	return 0;
}