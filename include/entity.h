#ifndef BULLSEYE_ENTITY_H
#define BULLSEYE_ENTITY_H

#include <string>
#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"
#include "reactphysics3d/reactphysics3d.h"

#include "mesh.h"
#include "shader.h"
#include "camera.h"

namespace bullseye::entity {
    enum class BodyType {
        NO_PHYSICS, COLLISION, RIGID
    };

    class Entity {
        public:
            Entity(std::string name, glm::vec3 position, rp3d::Quaternion orientation, BodyType body_type);
            ~Entity();

            void update(float delta_time);
            glm::mat4 get_model_matrix(float interp);
            virtual void unload(rp3d::PhysicsWorld* world);

            void set_mesh(std::string mesh_name);
            const char* get_name();
            const std::string& get_mesh_name();
            const glm::vec3& get_rotation();
            void set_rotation_speed(glm::vec3 rotation_speed);

            void init_physics(rp3d::PhysicsWorld* physics_world, rp3d::PhysicsCommon* physics_common, mesh::Mesh* mesh);

        protected:
            std::string name;
            std::string mesh_name;

            rp3d::PhysicsWorld* physics_world;
            rp3d::PhysicsCommon* physics_common;
            rp3d::CollisionBody* physics_body;
            BodyType body_type;

            glm::vec3 position;
            glm::vec3 previous_position;
            glm::vec3 rotation;
            glm::vec3 previous_rotation;

            rp3d::Transform previous_transform;

            glm::vec3 rotation_speed;
    };
}

#endif