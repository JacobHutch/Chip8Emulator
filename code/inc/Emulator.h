#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include <fstream>

class Emulator {
public:
	Emulator();
	void loadGame(std::string const& game);
	bool getRunningStatus();
private:
	bool mRunning;

	std::ifstream mRom;

	unsigned short mStack[16];
	unsigned short mStackPointer;

	unsigned char mMemory[4096];

	unsigned short mOpcode;

	unsigned char mRegister[16];
	unsigned short mIndexRegister;
	unsigned short mProgramCounter;

	unsigned char mGraphics[2048];
	bool mDrawFlag;

	unsigned char mDelayTimer;
	unsigned char mSoundTimer;

	unsigned char mKeyStates[16];
};
#endif