#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <queue>



class World {
    public:
    World(unsigned int x, unsigned int y);

    void genVertices(unsigned int x, unsigned int y);
    void genIndices(unsigned int x, unsigned int y);

    std::vector<float> getVertices();
    std::vector<int> getIndices();

    void draw();

    void pushColor(float color, unsigned int x, unsigned int y);

    private:
    unsigned int mWorldX;
    unsigned int mWorldY;
    std::vector<float> mVertices;
    std::vector<int> mIndices;
};