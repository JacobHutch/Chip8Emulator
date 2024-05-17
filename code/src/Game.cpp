#include "Game.h"
#include "DebugMsg.h"
#include "ShaderProgram.h"
#include "World.h"
#include "Emulator.h"
#include "Util.h"
#include "glinc.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <vector>
#include <chrono>



Emulator Game::mEmulator = Emulator();
bool Game::mPauseFlag = false;
bool Game::mResizeFlag = true;
const float Game::mTickRate = 1000.0f / 250;
const float Game::mDelayRate = 1000.0f / 60;
const float Game::mFrameRate = 1000.0f / 144;



Game::Game(int argc, char** argv) : mBackgroundColor(glm::vec3(0.1f, 0.1f, 0.1f)),mWinSize({1200,800}),
                                    mWorldSize({66,34}),mTitle("Emulator"),
                                    mArgc(argc), mArgv(argv) {}



void Game::setup() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    //glfwSwapInterval(1);
    mWindow = glfwCreateWindow(mWinSize.x, mWinSize.y, mTitle, nullptr, nullptr);
    if (mWindow == NULL) {
        Messenger::message(Messenger::Fatal, "Failed to open window!!");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, Game::framebufferSizeCallback);
    glfwSetKeyCallback(mWindow, Game::keyCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Messenger::message(Messenger::Fatal, "Failed to initialize GLAD!!");
        glfwTerminate();
        exit(-1);
    }

    std::string romLocations("./roms/");
    std::string defaultRom("pong");
    std::string fileExtension(".ch8");

    if(mArgc > 1) {
        Game::mEmulator.loadGame(romLocations + mArgv[1] + fileExtension);
    }
    else {
        std::cout << "No specified ROM, defaulting to " << defaultRom << "." << std::endl;
        Game::mEmulator.loadGame(romLocations + defaultRom + fileExtension);
    }
}



void Game::gameLoop() {
    setupShaders();
    generateBuffers();

    unsigned char* graphics = Game::mEmulator.getGraphics();
    int gx, gy;

    auto lastTime = std::chrono::steady_clock::now();
    auto delayLastTime = std::chrono::steady_clock::now();
    auto gpuLastTime = std::chrono::steady_clock::now();
    float deltaTime, delayDeltaTime, gpuDeltaTime;

    while (!glfwWindowShouldClose(mWindow) && Game::mEmulator.getRunningStatus()) {
        processInput(mWindow);

        if (Game::mResizeFlag) {
            Game::mResizeFlag = false;
            Game::resizeMath();
        }

        if (!Game::mPauseFlag) {
            auto delayCurrentTime = std::chrono::steady_clock::now();
            delayDeltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(delayCurrentTime - delayLastTime).count();
            if (delayDeltaTime > Game::mDelayRate) {
                delayLastTime = delayCurrentTime;
                mEmulator.decrementDelay();
            }

            auto currentTime = std::chrono::steady_clock::now();
            deltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTime).count();
            if (deltaTime > Game::mTickRate) {
                lastTime = currentTime;

                Game::mEmulator.cycle();
                if(Game::mEmulator.getDrawFlag()) {
                    Game::mEmulator.setDrawFlag(false);
                    for (int i = 0; i < 2048; i++) {
                        gx = i % 64;
                        gy = i / 64;
                        mWorld->pushColor(graphics[i],gx+1,gy+1);
                    }
                }
            }
        }

        auto gpuCurrentTime = std::chrono::steady_clock::now();
        gpuDeltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(gpuCurrentTime - gpuLastTime).count();
        if (gpuDeltaTime > Game::mFrameRate) {
            gpuLastTime = gpuCurrentTime;
            glClearColor(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b,1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            mWorld->draw();
            //glfwSwapBuffers(window);
            glFlush();
        }
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
    glDeleteProgram(mShader);
    delete mWorld;

    glfwTerminate();
}



void Game::setupShaders() {
    ShaderProgram squareShader = ShaderProgram("SquareProgram");
    squareShader.createShader(ShaderProgram::Vertex, "shaders/SquareShader.vs");
    squareShader.createShader(ShaderProgram::Fragment, "shaders/SquareShader.fs");
    squareShader.linkShaders();
    mShader = squareShader.getProgramID();
    squareShader.useProgram();
}



void Game::generateBuffers() {
    mWorld = new World(mWorldSize.x, mWorldSize.y);
    std::vector<float> vertices = mWorld->getVertices();
    std::vector<int> indices = mWorld->getIndices();

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}



void Game::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Game::mResizeFlag = true;
    glViewport(0, 0, width, height);
}



void Game::resizeMath() {
    int winWidth, winHeight;
    double tileX, tileY;
    glfwGetWindowSize(mWindow, &winWidth, &winHeight);

    double tileSizeX = ((double)winWidth) / (mWorldSize.x + 2);
    double tileSizeY = ((double)winHeight) / (mWorldSize.y + 2);
    if (tileSizeX < tileSizeY) {
        tileX = (double)mWorldSize.x + 2;
        tileY = ((double)mWorldSize.x + 2) * ((double)winHeight / winWidth);
    } else {
        tileX = ((double)mWorldSize.y + 2) * ((double)winWidth / winHeight);
        tileY = (double)mWorldSize.y + 2;
    }

    //tl (0,0), tr (1,0), bl (0,1), br (1,1)
    glm::mat4 view = glm::translate(glm::mat4(1.0), glm::vec3((tileX - mWorldSize.x) / 2.0, (tileY - mWorldSize.y) / 2.0, -1.0));
    glm::mat4 projection = glm::ortho(0.0, tileX, tileY, 0.0, 0.1, 100.0);
    glUniformMatrix4fv(glGetUniformLocation(mShader, "view"), 1, GL_FALSE, &(view[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(mShader, "proj"), 1, GL_FALSE, &(projection[0][0]));
}



void Game::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}



/*
Chip 8      Keyboard
1 2 3 C     4 5 6 7
4 5 6 D ==> R T Y U
7 8 9 E     F G H J
A 0 B F     V B N M
*/
void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "Key Pressed - " << glfwGetKeyName(key, 0) << std::endl;
        if (key == GLFW_KEY_4) {
            Game::mEmulator.setKeyState(1,1);
        } else if (key == GLFW_KEY_5) {
            Game::mEmulator.setKeyState(2,1);
        } else if (key == GLFW_KEY_6) {
            Game::mEmulator.setKeyState(3,1);
        } else if (key == GLFW_KEY_7) {
            Game::mEmulator.setKeyState(12,1);
        } else if (key == GLFW_KEY_R) {
            Game::mEmulator.setKeyState(4,1);
        } else if (key == GLFW_KEY_T) {
            Game::mEmulator.setKeyState(5,1);
        } else if (key == GLFW_KEY_Y) {
            Game::mEmulator.setKeyState(6,1);
        } else if (key == GLFW_KEY_U) {
            Game::mEmulator.setKeyState(13,1);
        } else if (key == GLFW_KEY_F) {
            Game::mEmulator.setKeyState(7,1);
        } else if (key == GLFW_KEY_G) {
            Game::mEmulator.setKeyState(8,1);
        } else if (key == GLFW_KEY_H) {
            Game::mEmulator.setKeyState(9,1);
        } else if (key == GLFW_KEY_J) {
            Game::mEmulator.setKeyState(14,1);
        } else if (key == GLFW_KEY_V) {
            Game::mEmulator.setKeyState(10,1);
        } else if (key == GLFW_KEY_B) {
            Game::mEmulator.setKeyState(0,1);
        } else if (key == GLFW_KEY_N) {
            Game::mEmulator.setKeyState(11,1);
        } else if (key == GLFW_KEY_M) {
            Game::mEmulator.setKeyState(15,1);
        } else if (key == GLFW_KEY_P) {
            Game::mPauseFlag = !Game::mPauseFlag;
        }
    } else if (action == GLFW_RELEASE) {
        std::cout << "Key Released - " << glfwGetKeyName(key, 0) << std::endl;
        if (key == GLFW_KEY_4) {
            Game::mEmulator.setKeyState(1,0);
        } else if (key == GLFW_KEY_5) {
            Game::mEmulator.setKeyState(2,0);
        } else if (key == GLFW_KEY_6) {
            Game::mEmulator.setKeyState(3,0);
        } else if (key == GLFW_KEY_7) {
            Game::mEmulator.setKeyState(12,0);
        } else if (key == GLFW_KEY_R) {
            Game::mEmulator.setKeyState(4,0);
        } else if (key == GLFW_KEY_T) {
            Game::mEmulator.setKeyState(5,0);
        } else if (key == GLFW_KEY_Y) {
            Game::mEmulator.setKeyState(6,0);
        } else if (key == GLFW_KEY_U) {
            Game::mEmulator.setKeyState(13,0);
        } else if (key == GLFW_KEY_F) {
            Game::mEmulator.setKeyState(7,0);
        } else if (key == GLFW_KEY_G) {
            Game::mEmulator.setKeyState(8,0);
        } else if (key == GLFW_KEY_H) {
            Game::mEmulator.setKeyState(9,0);
        } else if (key == GLFW_KEY_J) {
            Game::mEmulator.setKeyState(14,0);
        } else if (key == GLFW_KEY_V) {
            Game::mEmulator.setKeyState(10,0);
        } else if (key == GLFW_KEY_B) {
            Game::mEmulator.setKeyState(0,0);
        } else if (key == GLFW_KEY_N) {
            Game::mEmulator.setKeyState(11,0);
        } else if (key == GLFW_KEY_M) {
            Game::mEmulator.setKeyState(15,0);
        }
    }
}