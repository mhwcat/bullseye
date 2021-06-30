#ifndef BULLSEYE_GUN_H
#define BULLSEYE_GUN_H

#include <string>
#include "glm/glm.hpp"

#include "mesh.h"
#include "shader.h"
#include "entity.h"

namespace bullseye::entity::gun {
    const float SHOOT_ANIM_MAX_BACK_VAL = -0.035f;
    
    enum class GunState{
        IDLE, SHOOTING, RELOADING
    };

    enum class GunAnimState {
        IDLE, BACKING, RETURNING
    };

    class Gun : public Entity {
        public:
            Gun();

            void shoot();
            virtual void update(float delta_time);
            glm::mat4 get_model_matrix();
        private:
            glm::vec3 relative_position;
            GunState gun_state;
            GunAnimState gun_anim_state;
    };

}

#endif