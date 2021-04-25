#include "entity.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "math_utils.h"
#include "logger.h"

#include <string>
#include <vector>

#include "glm/gtc/matrix_transform.hpp"
#include "reactphysics3d/reactphysics3d.h"

namespace bullseye::entity {
    Entity::Entity(std::string name, glm::vec3 position, rp3d::Quaternion orientation, BodyType body_type) {
        this->name = name;
        this->position = position;
        this->previous_position = position;
        this->rotation = glm::vec3(0.f);
        this->previous_rotation = glm::vec3(0.f);
        this->rotation_speed = glm::vec3(0.f);
        this->body_type = body_type;

        this->previous_transform = rp3d::Transform(rp3d::Vector3(position.x, position.y, position.z), orientation);
    }

    Entity::~Entity() {

    }

    void Entity::init_physics(rp3d::PhysicsWorld* physics_world, rp3d::PhysicsCommon* physics_common) {
        assert(physics_common != nullptr);
        assert(physics_world != nullptr);

        this->physics_common = physics_common;
        this->physics_world = physics_world;

        logger::debug("Initializing physics for entity %s, transform[pos=[%.2f, %.2f, %.2f], orientation=[%.2f, %.2f, %.2f, %.2f]]",
            this->name.c_str(), this->previous_transform.getPosition().x, this->previous_transform.getPosition().y, this->previous_transform.getPosition().z,
            this->previous_transform.getOrientation().x, this->previous_transform.getOrientation().y, this->previous_transform.getOrientation().z, this->previous_transform.getOrientation().w);

        if (body_type != BodyType::NO_PHYSICS) {
            for (auto& mesh : this->meshes) {
                rp3d::BoxShape* boxShape = physics_common->createBoxShape(rp3d::Vector3(
                    mesh.second->get_extents().x, mesh.second->get_extents().y, mesh.second->get_extents().z));

                if (body_type == BodyType::RIGID) {
                    logger::debug("Adding rigid body for entity: %s, mesh: %s", this->name.c_str(), mesh.first.c_str());
                    
                    this->physics_body = physics_world->createRigidBody(this->previous_transform);
                    this->physics_body->addCollider(boxShape, rp3d::Transform(rp3d::Vector3(), rp3d::Quaternion::identity()));

                    if (this->name == "plane") {
                        dynamic_cast<rp3d::RigidBody*>(this->physics_body)->enableGravity(false);
                        dynamic_cast<rp3d::RigidBody*>(this->physics_body)->setType(rp3d::BodyType::STATIC);
                    }
                }

                if (body_type == BodyType::COLLISION) {
                    logger::debug("Adding collision body for entity: %s, mesh: %s", this->name.c_str(), mesh.first.c_str());

                    this->physics_body = physics_world->createCollisionBody(this->previous_transform);
                    this->physics_body->addCollider(boxShape, rp3d::Transform(rp3d::Vector3(), rp3d::Quaternion::identity()));
                }
            }
            
        }
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
        rp3d::Transform current_transform = this->physics_body->getTransform();
        rp3d::Transform interpolated_transform = rp3d::Transform::interpolateTransforms(this->previous_transform, current_transform, interp);
        this->previous_transform = current_transform;

        float model[16];

        for (auto &mesh : meshes) {
            interpolated_transform.getOpenGLMatrix(model);
            shader.set_mat4("model", model);

            mesh.second->draw(shader);
        }
    }

    void Entity::unload(rp3d::PhysicsWorld* world) {
        for (auto &mesh : this->meshes) {
            mesh.second->unload();
            delete mesh.second;
        }

        this->meshes.clear();

        // @TODO Fix crash on app exit
        if (world != nullptr && this->physics_body != nullptr) {
            if (this->body_type == BodyType::COLLISION) {
                world->destroyCollisionBody(this->physics_body);
            }

            if (this->body_type == BodyType::RIGID) {
                world->destroyRigidBody(dynamic_cast<rp3d::RigidBody*>(this->physics_body));
            }
        }
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
