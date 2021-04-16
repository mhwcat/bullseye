#version 430 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coords;

out vec3 fragment_position;
out vec3 normal;
out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragment_position = vec3(model * vec4(in_position, 1.0));
    normal = mat3(transpose(inverse(model))) * in_normal;  

    tex_coords = in_tex_coords;
    
    gl_Position = projection * view * vec4(fragment_position, 1.0);
}