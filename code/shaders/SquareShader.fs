#version 430 core

in float iColor;

vec3 oColor;

out vec4 FragColor;

void main() {
    if (iColor > 0.0f) {
        oColor = vec3(0.75f,0.8f,0.9f);
    } else {
        oColor = vec3(0.1f,0.1f,0.15f);
    }
    FragColor = vec4(oColor, 1.0f);
}