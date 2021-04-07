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

            void add_mesh_from_file(std::string mesh_name, std::string obj_path, glm::vec3 scale = glm::vec3(1.f));
            void add_mesh_from_array(std::string mesh_name, const float* vertices, const uint32_t vertices_len);
            void update(float delta_time);
            void draw(shader::Shader &shader, float interp);
            virtual void unload();

            const char* get_name();
        protected:
            std::string name;
            std::unordered_map<std::string, mesh::Mesh*> meshes;

            glm::vec3 position;
            glm::vec3 previous_position;
            glm::vec3 rotation;
            glm::vec3 previous_rotation;
    };
}

#endif