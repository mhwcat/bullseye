
#version 430 core

in vec4 vertex_color_out;
out vec4 color;

void main() {
    color = vertex_color_out;
}