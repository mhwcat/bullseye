
#version 430 core

// Uniform variables
uniform mat4 model;        // Local-space to world-space matrix
uniform mat4 view;       // World-space to camera-space matrix
uniform mat4 projection;          // Projection matrix

// In variables
in vec4 vertex_position;
in uint vertex_color;

// Out variables
out vec4 vertex_color_out;

void main() {
    // Compute the vertex position
    vec4 positionCameraSpace = view * model * vertex_position;

    // Compute the clip-space vertex coordinates
    gl_Position = projection * positionCameraSpace;

    // Transfer the vertex color to the fragment shader
    vertex_color_out = vec4((vertex_color & 0xFF0000u) >> 16, (vertex_color & 0x00FF00u) >> 8, vertex_color & 0x0000FFu, 0xFF);
}