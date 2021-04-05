#version 430 core

in vec3 position;
in vec3 normal;
//in vec2 tex_coords;

out vec3 v_position;
out vec3 v_normal;
//out vec2 v_tex_coords;

uniform mat4 perspective;
uniform mat4 view;
uniform mat4 model;

void main() {
    //mat4 model_view = view * model;
    mat4 model_view = model;
    v_normal = transpose(inverse(mat3(model_view))) * normal;
    gl_Position = perspective * model_view * vec4(position, 1.0);
    v_position = gl_Position.xyz / gl_Position.w;
    //v_tex_coords = tex_coords;
}