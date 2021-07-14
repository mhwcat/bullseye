#version 410 core

uniform mat4 proj;
uniform mat4 view;

in vec4 aPosition;

smooth out vec3 eyeDirection;

void main() {
    mat4 inverseProjection = inverse(proj);
    mat3 inverseModelview = transpose(mat3(view));
    vec3 unprojected = (inverseProjection * aPosition).xyz;
    eyeDirection = inverseModelview * unprojected;

    gl_Position = aPosition;
} 