#include "gun.h"
#include "logger.h"
#include "mesh.h"
#include "shader.h"
#include "math_utils.h"

#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glad/glad.h"
#include "reactphysics3d/reactphysics3d.h"

namespace bullseye::entity::gun {
    Gun::Gun() : Entity("gun", glm::vec3(0.f, 0.f, 0.f), rp3d::Quaternion::identity(), BodyType::NO_PHYSICS) {
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
                this->relative_position -= (glm::vec3(0.f, 0.f, 1.0f) * delta_time);

                if (this->relative_position.z <= gun::SHOOT_ANIM_MAX_BACK_VAL) {
                    this->gun_anim_state = GunAnimState::RETURNING;
                }
            }

            if (this->gun_anim_state == GunAnimState::RETURNING) {
                this->relative_position += (glm::vec3(0.f, 0.f, 0.5f) * delta_time);

                if (this->relative_position.z >= 0.f) {
                    this->relative_position.z = 0.f;
                    this->gun_anim_state = GunAnimState::IDLE;
                    this->gun_state = GunState::IDLE;
                }
            }
        }
    }

    glm::mat4 Gun::get_model_matrix() {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.07f, -0.08f, -0.2f));
        model = glm::rotate(model, math_utils::to_radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        //model = glm::scale(model, glm::vec3(0.016f, 0.016f, 0.016f));

        // Move model according to relative pos
        model = glm::translate(model, this->relative_position);

        return model;
    }
}