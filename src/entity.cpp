#include "entity.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "math_utils.h"
#include "logger.h"

#include <string>
#include <vector>
#include "glm/gtc/matrix_transform.hpp"

namespace bullseye::entity {
    Entity::Entity(std::string name, glm::vec3 position) {
        this->name = name;
        this->position = position;
        this->previous_position = position;
    }

    Entity::~Entity() {

    }

    void Entity::add_mesh_from_file(std::string mesh_name, std::string obj_path, glm::vec3 scale) {
        mesh::Mesh *mesh = new mesh::Mesh(mesh_name, obj_path, scale);
        this->meshes.insert({ mesh_name, mesh });
    }

    void Entity::add_mesh_from_array(std::string mesh_name, const float* vertices, const uint32_t vertices_len) {
        mesh::Mesh *mesh = new mesh::Mesh(mesh_name, vertices, vertices_len);
        this->meshes.insert({ mesh_name, mesh });
    }    

    void Entity::update(float delta_time) {
        
    }

    void Entity::draw(shader::Shader &shader, float interp) {
        for (auto &mesh : meshes) {
            glm::vec3 interpolated_pos = math_utils::lerp(this->previous_position, this->position, interp);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, interpolated_pos);

            shader.set_mat4("model", model);

            mesh.second->draw(shader);
        }
    }

    void Entity::unload() {
        for (auto &mesh : this->meshes) {
            mesh.second->unload();
            delete mesh.second;
        }

        this->meshes.clear();
    }

    const char* Entity::get_name() {
        return this->name.c_str();
    }
}
