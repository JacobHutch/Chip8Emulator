#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include "Emulator.h"

Emulator::Emulator() : mRunning(true), mStackPointer(0), mOpcode(0), mIndexRegister(0), mProgramCounter(0x200), mDrawFlag(true) {
  for(int i = 0; i < 4096; i++) {
		mMemory[i] = 0;
	}
  for(int i = 0; i < 2048; i++) {
    mGraphics[i] = 0;
  }
  for(int i = 0; i < 16; i++) {
    mStack[i] = 0;
    mRegister[i] = 0;
  }
  for(int i = 80; i < 160; i++) {
    mMemory[i] = mFontset[i];
  }
}

void Emulator::loadGame(std::string const& game) {
	mRom.open(game, std::ios::binary);
	if(mRom.fail()) {
		std::cout << "Error - no ROM file named " << game << " found!!" << std::endl;
		mRunning = false;
	}
	else {
		int i = mProgramCounter;
		while(mRom) {
			mRom >> mMemory[i + (((i % 2) * 2) - 1)];
            mGraphics[i] = mMemory[i];
			i += 1;
		}
	}
}

bool Emulator::getRunningStatus() {
	return mRunning;
}

void Emulator::cycle() {
	getOpcode();
	executeOpcode();

    if(mDelayTimer > 0) {
        mDelayTimer -= 1;
    }
    if(mSoundTimer > 0) {
        if(mSoundTimer == 1) {
            //sound
        }
        mSoundTimer -= 1;
    }
}

void Emulator::getOpcode() {
	mOpcode = (mMemory[mProgramCounter] << 8) | mMemory[mProgramCounter + 1];
}

void Emulator::executeOpcode() {
    switch(mOpcode & 0xF000) {

    case 0x0000:
        switch(mOpcode & 0x0FFF) {

        case 0x00E0:
            for (int i = 0; i < 2048; i++) {
                mGraphics[i] = '0';
            }
            mDrawFlag = true;
            mProgramCounter += 2;
            break;

        case 0x00EE:
            mStackPointer -= 1;
            mProgramCounter = mStack[mStackPointer];
            mProgramCounter += 2;
            break;

        default:
            mProgramCounter += 2;
            break;
        }

    case 0x1000:
        mProgramCounter = mOpcode & 0x0FFF;
        break;

    case 0x2000:
        mStack[mStackPointer] = mProgramCounter;
        mStackPointer += 1;
        mProgramCounter = mOpcode & 0x0FFF;
        break;

    case 0x3000:
        if(mRegister[(mOpcode & 0x0F00) >> 8] == (mOpcode & 0x00FF)) {
            mProgramCounter += 4;
        }
        else {
            mProgramCounter += 2;
        }
        break;

    case 0x4000:
        if (mRegister[(mOpcode & 0x0F00) >> 8] != (mOpcode & 0x00FF)) {
            mProgramCounter += 4;
        }
        else {
            mProgramCounter += 2;
        }
        break;

    case 0x5000:
        if (mRegister[(mOpcode & 0x0F00) >> 8] == mRegister[(mOpcode & 0x00F0) >> 4]) {
            mProgramCounter += 4;
        }
        else {
            mProgramCounter += 2;
        }
        break;

    case 0x6000:
        mRegister[(mOpcode & 0x0F00) >> 8] = mOpcode & 0x00FF;
        mProgramCounter += 2;
        break;

    case 0x7000:
        mRegister[(mOpcode & 0x0F00) >> 8] += mOpcode & 0x00FF;
        mProgramCounter += 2;
        break;

    case 0x8000:
        switch(mOpcode & 0x000F) {

        case 0x0000:
            mRegister[(mOpcode & 0x0F00) >> 8] = mRegister[(mOpcode & 0x00F0) >> 4];
            mProgramCounter += 2;
            break;

        case 0x0001:
            mRegister[(mOpcode & 0x0F00) >> 8] |= mRegister[(mOpcode & 0x00F0) >> 4];
            mProgramCounter += 2;
            break;

        case 0x0002:
            mRegister[(mOpcode & 0x0F00) >> 8] &= mRegister[(mOpcode & 0x00F0) >> 4];
            mProgramCounter += 2;
            break;

        case 0x0003:
            mRegister[(mOpcode & 0x0F00) >> 8] ^= mRegister[(mOpcode & 0x00F0) >> 4];
            mProgramCounter += 2;
            break;

        case 0x0004:
            if(mRegister[(mOpcode & 0x0F00) >> 8] + mRegister[(mOpcode & 0x00F0) >> 4] > 0xFF) {
                mRegister[15] = 1;
            }
            else {
                mRegister[15] = 0;
            }
            mRegister[(mOpcode & 0x0F00) >> 8] += mRegister[(mOpcode & 0x00F0) >> 4];
            mProgramCounter += 2;
            break;

        case 0x0005:
            if (mRegister[(mOpcode & 0x0F00) >> 8] < mRegister[(mOpcode & 0x00F0) >> 4]) {
                mRegister[15] = 1;
            }
            else {
                mRegister[15] = 0;
            }
            mRegister[(mOpcode & 0x0F00) >> 8] -= mRegister[(mOpcode & 0x00F0) >> 4];
            mProgramCounter += 2;
            break;

        case 0x0006:
            mRegister[15] = mRegister[(mOpcode & 0x0F00) >> 8] & 1;
            mRegister[(mOpcode & 0x0F00) >> 8] >>= 1;
            mProgramCounter += 2;
            break;

        case 0x0007:
            if (mRegister[(mOpcode & 0x00F0) >> 4] < mRegister[(mOpcode & 0x0F00) >> 8]) {
                mRegister[15] = 1;
            }
            else {
                mRegister[15] = 0;
            }
            mRegister[(mOpcode & 0x0F00) >> 8] = mRegister[(mOpcode & 0x00F0) >> 4] - mRegister[(mOpcode & 0x0F00) >> 8];
            mProgramCounter += 2;
            break;

        case 0x000E:
            mRegister[15] = mRegister[(mOpcode & 0x0F00) >> 8] & 0x80;
            mRegister[(mOpcode & 0x0F00) >> 8] <<= 1;
            mProgramCounter += 2;
            break;

        default:
            mGraphics[0] = 2;
            mDrawFlag = true;
            break;
        }

    case 0x9000:
        if (mRegister[(mOpcode & 0x0F00) >> 8] != mRegister[(mOpcode & 0x00F0) >> 4]) {
            mProgramCounter += 4;
        }
        else {
            mProgramCounter += 2;
        }
        break;

    case 0xA000:
        mIndexRegister = mOpcode & 0x0FFF;
        mProgramCounter += 2;
        break;

    case 0xB000:
        mProgramCounter = mRegister[0] + (mOpcode & 0x0FFF);
        break;

    case 0xC000:
        mRegister[(mOpcode & 0x0F00) >> 8] = rand() & (mOpcode & 0x00FF);
        mProgramCounter += 2;
        break;

    case 0xD000:
    {
        unsigned short rx = ((mOpcode & 0x0F00) >> 8) % 64;
        unsigned short ry = ((mOpcode & 0x00F0) >> 4) % 32;
        unsigned short height = (mOpcode & 0x000F) + 1;
        unsigned short row;
        unsigned short pixel;

        mRegister[15] = 0;
        for (int j = 0; j < height; j++) {
            row = mMemory[mIndexRegister + j];
            for (int i = 0; i < 8; i++) {
                pixel = (row & (1 << i));
                if (pixel != 0) {
                    if (mGraphics[(rx + i + ((ry + j) * 64))] == 1)
                    {
                        mRegister[15] = 1;
                    }
                    mGraphics[rx + i + ((ry + j) * 64)] ^= 1;
                }
            }
        }

        mDrawFlag = true;
        mProgramCounter += 2;
        break;
    }

    case 0xE000:
        switch(mOpcode & 0x00FF) {

        case 0x009E:
            if(mKeyStates[mRegister[(mOpcode & 0x0F00) >> 8]] != 0) {
                mProgramCounter += 4;
            }
            else {
                mProgramCounter += 2;
            }
            break;

        case 0x00A1:
            if (mKeyStates[mRegister[(mOpcode & 0x0F00) >> 8]] == 0) {
                mProgramCounter += 4;
            }
            else {
                mProgramCounter += 2;
            }
            break;

        default:
            mGraphics[0] = 2;
            mDrawFlag = true;
            break;
        }

    case 0xF000:
        switch(mOpcode & 0x00FF) {

        case 0x0007:
            mRegister[(mOpcode & 0x0F00) >> 8] = mDelayTimer;
            mProgramCounter += 2;
            break;

        case 0x000A:
            mProgramCounter += 2;
            break;

        case 0x0015:
            mDelayTimer = mRegister[(mOpcode & 0x0F00) >> 8];
            mProgramCounter += 2;
            break;

        case 0x0018:
            mSoundTimer = mRegister[(mOpcode & 0x0F00) >> 8];
            mProgramCounter += 2;
            break;

        case 0x001E:
            mIndexRegister += mRegister[(mOpcode & 0x0F00) >> 8];
            mProgramCounter += 2;
            break;

        case 0x0029:
            mIndexRegister = (mRegister[(mOpcode & 0x0F00) >> 8] * 5) + 80;
            mProgramCounter += 2;
            break;

        case 0x0033:
            mProgramCounter += 2;
            break;

        case 0x0055:
            for (int i = 0; i <= ((mOpcode & 0x0F00) >> 8); i++) {
                mMemory[mIndexRegister + i] = mRegister[i];
            }
            mIndexRegister += ((mOpcode & 0x0F00) >> 8) + 1;
            mProgramCounter += 2;
            break;

        case 0x0065:
            for(int i = 0; i <= ((mOpcode & 0x0F00) >> 8); i++) {
                mRegister[i] = mMemory[mIndexRegister + i];
            }
            mIndexRegister += ((mOpcode & 0x0F00) >> 8) + 1;
            mProgramCounter += 2;
            break;

        default:
            mGraphics[0] = 2;
            mDrawFlag = true;
            break;
        }
    
    default:
        break;
  }
}

bool Emulator::getDrawFlag() {
    return mDrawFlag;
}

void Emulator::setDrawFlag(bool flag) {
    mDrawFlag = flag;
}

unsigned char* Emulator::getGraphics() {
    return mGraphics;
}
