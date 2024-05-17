#include "World.h"

#include <glad/glad.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>



World::World(unsigned int x, unsigned int y) : mWorldX(x), mWorldY(y) {
    genVertices(x,y);
    genIndices(x,y);
}



void World::genVertices(unsigned int x, unsigned int y) {
    //each square has 4 vertices
    //vertex order in list: tl, tr, bl, br -- needs to be consistent with genIndices!!
    //vertices currently have position2 and color1, so multiply by 3 for stride
    std::vector<float> verts;
    for (unsigned int j = 0u; j < y; j++) {
        for (unsigned int i = 0u; i < x; i++) {

            //tl
            verts.push_back(0.0f + i);
            verts.push_back(0.0f + j);

            verts.push_back(0.0f);

            //tr
            verts.push_back(1.0f + i);
            verts.push_back(0.0f + j);

            verts.push_back(0.0f);

            //bl
            verts.push_back(0.0f + i);
            verts.push_back(1.0f + j);

            verts.push_back(0.0f);

            //br
            verts.push_back(1.0f + i);
            verts.push_back(1.0f + j);

            verts.push_back(0.0f);
        }
    }
    mVertices = verts;
}



void World::genIndices(unsigned int x, unsigned int y) {
    //as with genVertices, corner order is tl,tr,bl,br
    //triangles will be drawn as top left and bottom right (tl,bl,tr, tr,bl,br)
    std::vector<int> inds;
    int base, tl, tr, bl, br;
    for (unsigned int i = 0; i < x; i++) {
        for (unsigned int j = 0; j < y; j++) {

            base = (i + (j * x)) * 4;
            tl = base;
            tr = base + 1;
            bl = base + 2;
            br = base + 3;

            inds.push_back(tl);
            inds.push_back(bl);
            inds.push_back(tr);

            inds.push_back(tr);
            inds.push_back(bl);
            inds.push_back(br);
        }
    }
    mIndices = inds;
}



std::vector<float> World::getVertices() {
    return mVertices;
}



std::vector<int> World::getIndices() {
    return mIndices;
}



void World::draw() {
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
}



void World::pushColor(float color, unsigned int x, unsigned int y) {
    int tl, tr, bl, br;
    int size = sizeof(float);
    tl = ((((x + (y * mWorldX)) * 4) * 3) + 2) * size;
    tr = (((((x + (y * mWorldX)) * 4) + 1) * 3) + 2) * size;
    bl = (((((x + (y * mWorldX)) * 4) + 2) * 3) + 2) * size;
    br = (((((x + (y * mWorldX)) * 4) + 3) * 3) + 2) * size;
    glBufferSubData(GL_ARRAY_BUFFER, tl, size, &color);
    glBufferSubData(GL_ARRAY_BUFFER, tr, size, &color);
    glBufferSubData(GL_ARRAY_BUFFER, bl, size, &color);
    glBufferSubData(GL_ARRAY_BUFFER, br, size, &color);
}
