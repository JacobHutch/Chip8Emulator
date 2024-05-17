#pragma once

#include <vector>
#include <cstring>



class ShaderProgram {
    public:
    enum shaderType {
        Vertex,
        Fragment,
        Compute
    };

    ShaderProgram(const char* programName);

    void createShader(shaderType type, const char* shaderSource);
    void linkShaders();
    unsigned int getProgramID();
    void useProgram();



    private:
    unsigned int mCount;
    std::vector<unsigned int> mShadersList;
    unsigned int mProgram;
    const char* mProgramName;
};