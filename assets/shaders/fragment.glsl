#version 430 core 

out vec4 frag_color;
uniform vec4 in_color;

void main() {
    frag_color = in_color;
}