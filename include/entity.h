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

            void add_mesh_from_file(std::string mesh_name, std::string obj_path, glm::vec3 scale = glm::vec3(1.f));
            void add_mesh_from_array(std::string mesh_name, const float* vertices, const uint32_t vertices_len);
            void update(float delta_time);
            void draw(shader::Shader &shader, float interp);
            virtual void unload(rp3d::PhysicsWorld* world);

            const char* get_name();
            const glm::vec3& get_rotation();
            void set_rotation_speed(glm::vec3 rotation_speed);

            void init_physics(rp3d::PhysicsWorld* physics_world, rp3d::PhysicsCommon* physics_common);

        protected:
            std::string name;
            std::unordered_map<std::string, mesh::Mesh*> meshes;

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