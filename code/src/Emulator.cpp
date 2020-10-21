#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Emulator.h"

Emulator::Emulator() : mRunning(true) {
}

void Emulator::loadGame(std::string const& game) {
	mRom.open(game, std::ios::binary);
	if(mRom.fail()) {
		std::cout << "Error - no ROM file named " << game << " found!!" << std::endl;
		mRunning = false;
	}
	else {
		int i = 512;
		while(mRom) {
			mRom >> mMemory[i];
			i++;
		}
		for(i=0;i<4096;i++) {
			std::cout << std::hex << (int)mMemory[i] << std::endl;
		}
	}
}

bool Emulator::getRunningStatus() {
	return mRunning;
}