#ifndef BULLSEYE_GUN_H
#define BULLSEYE_GUN_H

#include <string>
#include <glm/glm.hpp>

#include "mesh.h"
#include "shader.h"

namespace bullseye::gun {
    const float SHOOT_ANIM_MAX_BACK_VAL = -1.5f;
    
    enum class GunState{
        IDLE, SHOOTING, RELOADING
    };

    enum class GunAnimState {
        IDLE, BACKING, RETURNING
    };

    class Gun {
        public:
            Gun(std::string model_path, std::string vertex_shader_path, std::string fragment_shader_path);

            void shoot();
            void update(float delta_time);
            void draw(glm::mat4 projection, glm::mat4 view);
            void unload();
        private:
            mesh::Mesh* mesh;
            shader::Shader* shader;
            mesh::Mesh* projectile;

            glm::vec3 relative_position;
            GunState gun_state;
            GunAnimState gun_anim_state;
    };

}

#endif