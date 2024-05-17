#pragma once

#include "glinc.h"
#include <glm/glm.hpp>
#include <vector>



typedef struct _node {
    std::vector<int> domain;
    glm::vec3 color;
    int entropy;
    glm::vec2 pos;
    bool queued;
    struct _node* left;
    struct _node* right;
    struct _node* up;
    struct _node* down;
} node;




namespace Util {
    double lerp(double a, double b, double t);
    glm::vec3 lerp3D(glm::vec3 a, glm::vec3 b, double t);
    bool NodeCompare(node* a, node* b);
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
}
