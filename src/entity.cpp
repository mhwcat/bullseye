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
        this->rotation = glm::vec3(0.f);
        this->previous_rotation = glm::vec3(0.f);
        this->rotation_speed = glm::vec3(0.f);
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
        glm::vec3 rotation_velocity = this->rotation_speed * delta_time;

        previous_rotation.x = rotation.x;
        previous_rotation.y = rotation.y;
        previous_rotation.z = rotation.z;

        this->rotation += rotation_velocity;
    }

    void Entity::draw(shader::Shader &shader, float interp) {
        glm::vec3 interpolated_pos = math_utils::lerp(this->previous_position, this->position, interp);
        //glm::vec3 interpolated_rot = math_utils::lerp(this->previous_rotation, this->rotation, interp);

        for (auto &mesh : meshes) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, interpolated_pos);
            model = glm::rotate(model, math_utils::to_radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
            model = glm::rotate(model, math_utils::to_radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
            model = glm::rotate(model, math_utils::to_radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));

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

    const glm::vec3& Entity::get_rotation() {
        return this->rotation;
    }

    void Entity::set_rotation_speed(glm::vec3 rotation_speed) {
        this->rotation_speed = rotation_speed;
    }
}
