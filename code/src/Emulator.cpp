#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

#include "Emulator.h"
#include "DebugMsg.h"



Emulator::Emulator() : mRunning(true), mStackPointer(0), mOpcode(0), mIndexRegister(0), mProgramCounter(0x200), mDrawFlag(true), mDelayTimer(0), mSoundTimer(0) {
    srand(time(NULL));
    for(int i = 0; i < 4096; i++) {
        mMemory[i] = 0;
    }
    for(int i = 0; i < 2048; i++) {
        mGraphics[i] = 0;
    }
    for(int i = 0; i < 16; i++) {
        mStack[i] = 0;
        mRegister[i] = 0;
        mKeyStates[i] = 0;
    }
    for(int i = 0; i < 80; i++) {
        mMemory[i + 80] = mFontset[i];
    }
}



void Emulator::loadGame(std::string const& game) {
    mRom.open(game, std::ios::in | std::ios::binary);
    if(mRom.fail()) {
        Messenger::message(Messenger::Error, "No ROM file named " + game + " found!!");
        mRunning = false;
    }
    else {
        int i = mProgramCounter;
        unsigned char temp = mRom.get();
        while(!mRom.eof()) {
            mMemory[i] = temp;
            i += 1;
            temp = mRom.get();
        }
        for (int j = 0; j < 4096; j+=2) {
            std::cout << "0x" << std::setfill('0') << std::setw(3) << std::hex << j
                << " - " << std::setfill('0') << std::setw(4) << std::dec << j << " - "
                << "0x" << std::setfill('0') << std::setw(4) << std::hex << ((mMemory[j] << 8) | mMemory[j + 1]) << std::endl;
        }
    }
    mRom.close();
}



bool Emulator::getRunningStatus() {
    return mRunning;
}



void Emulator::cycle() {
    getOpcode();
    //std::cout << "Opcode - 0x" << std::setfill('0') << std::setw(4) << std::hex << mOpcode << std::endl;
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
                mGraphics[i] = 0;
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
            break;
        }
        break;

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
            break;
        }
        break;

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
        mRegister[(mOpcode & 0x0F00) >> 8] = (rand() % 0xFF) & (mOpcode & 0x00FF);
        mProgramCounter += 2;
        break;

    case 0xD000:
        {
        unsigned char sx = mRegister[(mOpcode & 0x0F00) >> 8];
        unsigned char sy = mRegister[(mOpcode & 0x00F0) >> 4];
        unsigned char x,y;
        unsigned char length = (mOpcode & 0x000F);
        unsigned char byte;
        unsigned char pixel;
        int pos;

        mRegister[15] = 0;
        for (int i = 0; i < length; i++) {
            byte = mMemory[mIndexRegister + i];
            y = (sy + i) % 32;
            for (int j = 0; j < 8; j++) {
                x = (sx + j) % 64;
                pos = flattenPos(x,y);
                pixel = (byte & (1 << (7 - j)));
                if (pixel != 0) {
                    if ((pos >= 0) && (mGraphics[pos] == 1)) {
                        mRegister[15] = 1;
                    }
                    mGraphics[pos] ^= 1;
                }
            }
        }

        mDrawFlag = true;
        mProgramCounter += 2;
        }
        break;

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
            break;
        }
        break;

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
            mMemory[mIndexRegister] = mRegister[(mOpcode & 0x0F00) >> 8] / 100;
            mMemory[mIndexRegister + 1] = (mRegister[(mOpcode & 0x0F00) >> 8] / 10) % 10;
            mMemory[mIndexRegister + 2] = (mRegister[(mOpcode & 0x0F00) >> 8] % 100) % 10;
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
            break;
        }
        break;

    default:
        Messenger::message(Messenger::Warning, "Really not sure how you got here.");
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



// inserts byte horizontally in big endian form
void Emulator::byteToGraphics(unsigned char val, int pos) {
    for (int i = 0; i < 8; i++) {
        mGraphics[pos + i] ^= (val >> (7 - i)) & 1;
    }
}



void Emulator::setKeyState(int key, unsigned char state) {
    mKeyStates[key] = state;
}



int Emulator::flattenPos(int x, int y) {
    int ret;
    if ((x < 64) && (y < 32)) {
        ret = (y * 64) + x;
    } else {
        ret = -1;
    }
    return ret;
}



void Emulator::decrementDelay() {
    //std::cout << "Delay - " << std::dec << static_cast<int>(mDelayTimer) << std::endl;
    //std::cout << "Sound - " << static_cast<int>(mSoundTimer) << std::endl;
    if (mDelayTimer > 0) {
        std::cout << "Delay - " << std::dec << static_cast<int>(mDelayTimer) << std::endl;
        mDelayTimer -= 1;
    }
    if (mSoundTimer > 0) {
        std::cout << "Sound - " << std::dec << static_cast<int>(mSoundTimer) << std::endl;
        mSoundTimer -= 1;
    }
}