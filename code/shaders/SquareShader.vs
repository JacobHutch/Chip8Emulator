#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in float aColor;

uniform mat4 proj;
uniform mat4 view;

out float iColor;

void main() {
    gl_Position = proj * view * vec4(aPos,0.0f,1.0f);
    iColor = aColor;
}