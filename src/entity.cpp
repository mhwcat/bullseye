#include "entity.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "math_utils.h"
#include "logger.h"

#include <string>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace bullseye::entity {
    Entity::Entity(std::string name, glm::vec3 position) {
        this->name = name;
        this->position = position;
        this->previous_position = position;
    }

    Entity::~Entity() {

    }

    void Entity::add_shader(std::string name, std::string vertex_shader_path, std::string fragment_shader_path) {
        shader::Shader *shader = new shader::Shader(name);
        shader->load_vertex_shader(vertex_shader_path.c_str());
        shader->load_fragment_shader(fragment_shader_path.c_str());
        shader->link_shaders();

        this->shaders.insert({ name, shader });
    }

    void Entity::add_mesh_from_file(std::string mesh_name, std::string obj_path) {
        mesh::Mesh *mesh = new mesh::Mesh(mesh_name, obj_path);
        this->meshes.insert({ mesh_name, mesh });
    }

    void Entity::add_mesh_from_array(std::string mesh_name, const float* vertices, const uint32_t vertices_len) {
        mesh::Mesh *mesh = new mesh::Mesh(mesh_name, vertices, vertices_len);
        this->meshes.insert({ mesh_name, mesh });
    }    

    void Entity::map_mesh_to_shader(std::string mesh_name, std::string shader_name) {
        if (this->meshes_to_shaders_map.find(mesh_name) != this->meshes_to_shaders_map.end()) {
            std::string current_shader = this->meshes_to_shaders_map[shader_name];
            logger::warn("Mesh %s is already mapped to shader %s, value will be replaced with shader %s", mesh_name.c_str(), current_shader.c_str(), shader_name.c_str());

            this->meshes_to_shaders_map.erase(mesh_name);
        }

        this->meshes_to_shaders_map.insert({ mesh_name, shader_name });
    }

    void Entity::update(float delta_time) {
        
    }

    void Entity::draw(camera::Camera &camera, glm::vec3 light_pos, float interp) {
        for (auto &mesh : meshes) {
            std::string shader_name = this->meshes_to_shaders_map.at(std::string(mesh.second->get_name()));
            shader::Shader* shader = this->shaders.at(shader_name);

            shader->use();
            shader->set_mat4("projection", camera.get_perspective_matrix());
            shader->set_mat4("view", camera.get_view_matrix(interp));
            shader->set_vec3("light_pos", light_pos);
            shader->set_vec3("view_pos", *camera.get_position());
            shader->set_vec3("light_color", glm::vec3(1.f, 1.f, 1.f));
            shader->set_vec3("object_color", glm::vec3(0.1f, 0.5f, 0.3f));  

            glm::vec3 interpolated_pos = math_utils::lerp(this->previous_position, this->position, interp);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, interpolated_pos);

            shader->set_mat4("model", model);

            mesh.second->draw(*shader);
        }
    }

    void Entity::unload() {
        for (auto &mesh : this->meshes) {
            mesh.second->unload();
            delete mesh.second;
        }

        this->meshes.clear();

        for (auto &shader : this->shaders) {
            shader.second->delete_program();
            delete shader.second;
        }

        this->shaders.clear();
    }
}
