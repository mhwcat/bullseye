#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <SDL.h>

namespace bullseye::camera {
    Camera::Camera(uint32_t screen_width, uint32_t screen_height) {
        position = glm::vec3(0.f, 0.f, 0.f);
        previous_position = glm::vec3(0.f, 0.f, 0.f);

        up = glm::vec3(0.0f, 1.0f, 0.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        right = glm::vec3(1.0f, 0.0f, 0.0f);
        world_up = glm::vec3(0.0f, 1.0f, 0.0f);
        yaw = -90.0f;
        pitch = 0.0f;

        movement_speed = DEFAULT_MOVEMENT_SPEED;
        movement_direction = NONE;
        sensitivity = 0.1f;

        mouse_attached = false;

        update_aspect_ratio(screen_width, screen_height);
        update_camera_vectors();
    }

    void Camera::update_aspect_ratio(uint32_t screen_width, uint32_t screen_height) {
        aspect_ratio = (float) screen_width / (float) screen_height;
    }

    void Camera::update_movement_speed(float movement_speed_delta) {
        if (movement_speed + movement_speed_delta < 0.0f) {
            movement_speed = 0.0f;
        } else {
            movement_speed += movement_speed_delta;
        }
    }

    void Camera::update_camera_vectors() {
        glm::vec3 new_front = glm::normalize(glm::vec3(
            cos(to_radians(yaw)) * cos(to_radians(pitch)),
            sin(to_radians(pitch)),
            sin(to_radians(yaw)) * cos(to_radians(pitch))
        ));

        glm::vec3 current_world_up = glm::vec3(world_up.x, world_up.y, world_up.z);
        glm::vec3 new_right = glm::normalize(glm::cross(new_front, current_world_up));
        glm::vec3 new_up = glm::normalize(glm::cross(new_right, new_front));

        front.x = new_front.x;
        front.y = new_front.y;
        front.z = new_front.z;

        right.x = new_right.x;
        right.y = new_right.y;
        right.z = new_right.z;

        up.x = new_up.x;
        up.y = new_up.y;
        up.z = new_up.z;
    }

    void Camera::process_mouse_input(float x_offset, float y_offset) {
        float x_offset_adjusted = x_offset * sensitivity;
        float y_offset_adjusted = y_offset * sensitivity;

        yaw += x_offset_adjusted;
        pitch += y_offset_adjusted;

        update_camera_vectors();
    }

    void Camera::process_input(MovementDirection _movement_direction) {
        movement_direction = _movement_direction;
    }

    void Camera::update(float delta_time) {
        float velocity = movement_speed * delta_time;

        previous_position.x = position.x;
        previous_position.y = position.y;
        previous_position.z = position.z;

        switch (this->movement_direction) {
            case UP:
                position += up * velocity;
                break;
            case DOWN:
                position -= up * velocity;
                break;
            case FRONT:
                position += front * velocity;
                break;
            case BACK:
                position -= front * velocity;
                break;
            case LEFT:
                position -= right * velocity;
                break;
            case RIGHT:
                position += right * velocity;
                break;
            default:
                break;
        }
    }

    glm::mat4 Camera::get_perspective_matrix() {
        return glm::perspective(FOV, aspect_ratio, Z_NEAR, Z_FAR);
    }

    glm::mat4 Camera::get_view_matrix(float interp) {
        // TODO: Find lerp in glm and use it
        glm::vec3 interpolated_pos = lerp(previous_position, position, interp);

        return glm::lookAt(interpolated_pos, (interpolated_pos + front), up);
    }

    const glm::vec3* Camera::get_position() {
        return &this->position;
    }

    const bool Camera::is_mouse_attached() {
        return this->mouse_attached;
    }

    void Camera::switch_mouse_attached() {
        this->mouse_attached = !this->mouse_attached;

        if (this->mouse_attached) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }
        else {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
    }

}