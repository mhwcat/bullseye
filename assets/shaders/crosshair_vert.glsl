#version 330 core
layout (location = 0) in vec3 position; // <vec2 pos, vec2 tex>

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position.xy, 0.0, 1.0);
    //TexCoords = vertex.zw;
}  