#version 430 core

in vec3 normal;
in vec3 fragment_position;

//in vec2 v_tex_coords;

out vec4 color;

uniform vec3 light_pos;
uniform vec3 view_pos;
uniform vec3 light_color;
uniform vec3 object_color;
//uniform sampler2D tex;

const float ambient_strength = 0.2;
const float specular_strength = 0.8;

void main() {
    vec3 ambient = ambient_strength * light_color;

    vec3 normalized_normal = normalize(normal);
    vec3 light_direction = normalize(light_pos - fragment_position);
    float diff = max(dot(normalized_normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 view_direction = normalize(view_pos - fragment_position);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    vec3 final = (ambient + diffuse + specular) * object_color;

    color = vec4(final, 1.0);    
}