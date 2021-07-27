#include "entity.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "math_utils.h"
#include "clogger.h"

#include <string>
#include <vector>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
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

    void Entity::init_physics(rp3d::PhysicsWorld* physics_world, rp3d::PhysicsCommon* physics_common, mesh::Mesh* mesh, float mass) {
        assert(physics_common != nullptr);
        assert(physics_world != nullptr);

        this->physics_common = physics_common;
        this->physics_world = physics_world;

        CLOG_DEBUG("Initializing physics for entity %s, transform[pos=[%.2f, %.2f, %.2f], orientation=[%.2f, %.2f, %.2f, %.2f]]",
            this->name.c_str(), this->previous_transform.getPosition().x, this->previous_transform.getPosition().y, this->previous_transform.getPosition().z,
            this->previous_transform.getOrientation().x, this->previous_transform.getOrientation().y, this->previous_transform.getOrientation().z, this->previous_transform.getOrientation().w);

        if (body_type != BodyType::NO_PHYSICS) {
            rp3d::BoxShape* boxShape = physics_common->createBoxShape(rp3d::Vector3(
                mesh->get_extents().x, mesh->get_extents().y, mesh->get_extents().z));

            if (body_type == BodyType::RIGID) {
                CLOG_DEBUG("Adding rigid body for entity: %s, mesh: %s", this->name.c_str(), mesh->get_name());
                    
                this->physics_body = physics_world->createRigidBody(this->previous_transform);
                this->physics_body->addCollider(boxShape, rp3d::Transform(rp3d::Vector3(), rp3d::Quaternion::identity()));
                if (mass > 0.f) {
                    dynamic_cast<rp3d::RigidBody*>(this->physics_body)->setMass(mass);
                }

                if (this->name == "plane") {
                    dynamic_cast<rp3d::RigidBody*>(this->physics_body)->enableGravity(false);
                    dynamic_cast<rp3d::RigidBody*>(this->physics_body)->setType(rp3d::BodyType::STATIC);
                }
            }

            if (body_type == BodyType::COLLISION) {
                CLOG_DEBUG("Adding collision body for entity: %s, mesh: %s", this->name.c_str(), mesh->get_name());

                this->physics_body = physics_world->createCollisionBody(this->previous_transform);
                this->physics_body->addCollider(boxShape, rp3d::Transform(rp3d::Vector3(), rp3d::Quaternion::identity()));
            }
            
        }
    }

    void Entity::update(float delta_time) {
        glm::vec3 rotation_velocity = this->rotation_speed * delta_time;

        previous_rotation.x = rotation.x;
        previous_rotation.y = rotation.y;
        previous_rotation.z = rotation.z;

        this->rotation += rotation_velocity;

        if (this->body_type == BodyType::RIGID) {
            dynamic_cast<rp3d::RigidBody*>(this->physics_body)->applyForceToCenterOfMass(this->applied_force);
        }
    }

    glm::mat4 Entity::get_model_matrix(float interp) {
        rp3d::Transform current_transform = this->physics_body->getTransform();
        rp3d::Transform interpolated_transform = rp3d::Transform::interpolateTransforms(this->previous_transform, current_transform, interp);
        this->previous_transform = current_transform;

        float model_matrix[16];
        interpolated_transform.getOpenGLMatrix(model_matrix);

        return glm::make_mat4(model_matrix);
    }

    void Entity::unload(rp3d::PhysicsWorld* world) {
        if (world != nullptr && this->physics_body != nullptr) {
            if (this->body_type == BodyType::COLLISION) {
                world->destroyCollisionBody(this->physics_body);
            }

            if (this->body_type == BodyType::RIGID) {
                world->destroyRigidBody(dynamic_cast<rp3d::RigidBody*>(this->physics_body));
            }
        }
    }

    void Entity::set_mesh(std::string mesh_name) {
        this->mesh_name = mesh_name;
    }

    const char* Entity::get_name() {
        return this->name.c_str();
    }

    const std::string& Entity::get_mesh_name() {
        return this->mesh_name;
    }

    const glm::vec3& Entity::get_position() {
        return this->position;
    }

    const glm::vec3& Entity::get_rotation() {
        return this->rotation;
    }

    void Entity::set_rotation_speed(glm::vec3 rotation_speed) {
        this->rotation_speed = rotation_speed;
    }

    rp3d::CollisionBody* Entity::get_collision_body() {
        return this->physics_body;
    }

    // Warning: this will crash for any body that is not rigid 
    rp3d::RigidBody* Entity::get_rigid_body() {
        return dynamic_cast<rp3d::RigidBody*>(this->physics_body);
    }

    void Entity::set_force(glm::vec3 force) {
        this->applied_force = rp3d::Vector3(force.x, force.y, force.z);
    }
}
