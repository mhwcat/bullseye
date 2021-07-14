#version 410 core

uniform samplerCube skybox;

smooth in vec3 eyeDirection;

out vec4 color;

void main() {
    color = texture(skybox, eyeDirection);
}