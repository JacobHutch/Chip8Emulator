#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include <fstream>



class Emulator {

    public:
    Emulator();
    void loadGame(std::string const& game);
    bool getRunningStatus();
    void cycle();
    void getOpcode();
    void executeOpcode();
    bool getDrawFlag();
    void setDrawFlag(bool flag);
    unsigned char* getGraphics();
    void byteToGraphics(unsigned char val, int pos);
    void setKeyState(int key, unsigned char state);
    int flattenPos(int x, int y);
    void decrementDelay();



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

    //large span is rows, small is columns
    //ex: r0c0,r0c1,r0c2,...,r4c31,r5c0,...
    unsigned char mGraphics[2048];
    bool mDrawFlag;

    unsigned char mDelayTimer;
    unsigned char mSoundTimer;

    unsigned char mKeyStates[16];

    unsigned char mFontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
  };
};



#endif
