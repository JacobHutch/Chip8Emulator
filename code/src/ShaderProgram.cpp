#include "ShaderProgram.h"
#include "DebugMsg.h"

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>



ShaderProgram::ShaderProgram(const char* programName) : mProgramName(programName) {
    mCount = 0;
    mShadersList = std::vector<unsigned int>();
    mProgram = 0;
}



void ShaderProgram::createShader(shaderType type, const char* shaderSource) {
    unsigned int shader = 0;
    std::ifstream inFile;
    inFile.open(shaderSource);
    std::stringstream fileStream;
    fileStream << inFile.rdbuf();
    std::string shaderString = fileStream.str();
    const char* shaderChar = shaderString.c_str();

    switch (type) {
        case Vertex:
            shader = glCreateShader(GL_VERTEX_SHADER);
            break;
        case Fragment:
            shader = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case Compute:
            shader = glCreateShader(GL_COMPUTE_SHADER);
            break;
        default:
            Messenger::message(Messenger::Error, "Invalid shader type!!");
            return;
    }

    glShaderSource(shader, 1, &shaderChar, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string error = "Shader compilation failed!!\n\tSource: ";
        error.append(shaderSource);
        error.append("\n\tProgram: ");
        error.append(mProgramName);
        error.append("\n\tLog:\n");
        error.append(infoLog);
        Messenger::message(Messenger::Error, error);
        exit(-1);
    }
    mShadersList.push_back(shader);
    mCount += 1;
}



void ShaderProgram::linkShaders() {
    unsigned int shaderProgram = glCreateProgram();

    for (unsigned int i = 0; i < mCount; i++) {
        glAttachShader(shaderProgram, mShadersList[i]);
    }

    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::string error = "Shader linking failed!!\n\tProgram: ";
        error.append(mProgramName);
        error.append("\n\tLog:\n");
        error.append(infoLog);
        Messenger::message(Messenger::Error, error);
        return;
    }

    for (unsigned int i = 0; i < mCount; i++) {
        glDeleteShader(mShadersList[i]);
    }
    mProgram = shaderProgram;
}



unsigned int ShaderProgram::getProgramID() {
    return mProgram;
}



void ShaderProgram::useProgram() {
    if (mProgram == 0) {
        std::string warn = "Shader program \"";
        warn.append(mProgramName);
        warn.append("\" does not exist!!");
        Messenger::message(Messenger::Warning, warn);
    }
    glUseProgram(mProgram);
}