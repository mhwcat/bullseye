#ifndef BULLSEYE_ENTITY_H
#define BULLSEYE_ENTITY_H

#include <string>
#include <vector>
#include <unordered_map>
#include "glm/glm.hpp"

#include "mesh.h"
#include "shader.h"
#include "camera.h"

namespace bullseye::entity {
    class Entity {
        public:
            Entity(std::string name, glm::vec3 position);
            ~Entity();

            void add_mesh_from_file(std::string mesh_name, std::string obj_path);
            void add_mesh_from_array(std::string mesh_name, const float* vertices, const uint32_t vertices_len);
            void add_shader(std::string name, std::string vertex_shader_path, std::string fragment_shader_path);
            void map_mesh_to_shader(std::string mesh_name, std::string shader_name);
            void update(float delta_time);
            void draw(camera::Camera& camera, glm::vec3 light_pos, float interp);
            virtual void unload();

            const char* get_name();
        private:
            std::string name;
            std::unordered_map<std::string, mesh::Mesh*> meshes;
            std::unordered_map<std::string, shader::Shader*> shaders;
            std::unordered_map<std::string, std::string> meshes_to_shaders_map;

            glm::vec3 position;
            glm::vec3 previous_position;
            glm::vec3 rotation;
            glm::vec3 previous_rotation;
    };
}

#endif