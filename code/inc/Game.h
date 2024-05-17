#pragma once

#include "glinc.h"
#include "Emulator.h"
#include "World.h"

#include <glm/glm.hpp>
#include <string>



class Game {
    public:
    Game(int argc, char** argv);

    void setup();
    void gameLoop();
    void setupShaders();
    void generateBuffers();
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void processInput(GLFWwindow* window);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void resizeMath();
    static Emulator mEmulator;
    static const float mTickRate;
    static const float mDelayRate;
    static const float mFrameRate;

    private:
    GLFWwindow* mWindow;
    unsigned int mShader;
    World* mWorld;
    unsigned int mVBO, mVAO, mEBO;
    static bool mPauseFlag;
    static bool mResizeFlag;

    const glm::vec3 mBackgroundColor;
    const glm::uvec2 mWinSize;
    const glm::uvec2 mWorldSize;
    const char* mTitle;
    int mArgc;
    char** mArgv;
};