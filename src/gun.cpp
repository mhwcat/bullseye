#include "gun.h"
#include "logger.h"
#include "mesh.h"
#include "shader.h"
#include "math_utils.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace bullseye::gun {
    Gun::Gun(std::string model_path, std::string vertex_shader_path, std::string fragment_shader_path) {
        this->mesh = new mesh::Mesh("gun", model_path);

        this->shader = new shader::Shader("gun");
        this->shader->load_vertex_shader(vertex_shader_path.c_str());
        this->shader->load_fragment_shader(fragment_shader_path.c_str());
        this->shader->link_shaders();

        //this->projectile = new mesh::Mesh();

        this->relative_position = glm::vec3(0.f, 0.f, 0.f);
        this->gun_state = GunState::IDLE;
        this->gun_anim_state = GunAnimState::IDLE;
    }

    void Gun::shoot() {
        this->gun_state = GunState::SHOOTING;
        this->gun_anim_state = GunAnimState::BACKING;
    }

    void Gun::update(float delta_time) {
        if (this->gun_state == GunState::SHOOTING) {
            if (this->gun_anim_state == GunAnimState::BACKING) {
                this->relative_position -= (glm::vec3(0.f, 0.f, 50.0f) * delta_time);

                if (this->relative_position.z <= gun::SHOOT_ANIM_MAX_BACK_VAL) {
                    this->gun_anim_state = GunAnimState::RETURNING;
                }
            }

            if (this->gun_anim_state == GunAnimState::RETURNING) {
                this->relative_position += (glm::vec3(0.f, 0.f, 15.0f) * delta_time);

                if (this->relative_position.z >= 0.f) {
                    this->relative_position.z = 0.f;
                    this->gun_anim_state = GunAnimState::IDLE;
                    this->gun_state = GunState::IDLE;
                }
            }
        }
    }

    void Gun::draw(glm::mat4 proj, glm::mat4 view) {
        shader->use();
        shader->set_mat4("perspective", proj);
        shader->set_mat4("view", view);
        glm::vec3 light = glm::vec3(2.4f, 2.4f, -1.7f);
        shader->set_vec3("u_light", light);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.07f, -0.08f, -0.2f));
        model = glm::rotate(model, math_utils::to_radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        model = glm::scale(model, glm::vec3(0.016f, 0.016f, 0.016f));

        // Move model according to relative pos
        model = glm::translate(model, this->relative_position);

        shader->set_mat4("model", model);

        this->mesh->draw(*this->shader);
    }

    void Gun::unload() {
        if (this->mesh != nullptr) {
            this->mesh->unload();
            delete this->mesh;
        }

        if (this->shader != nullptr) {
            this->shader->delete_program();
            delete this->shader;
        }
    }
}