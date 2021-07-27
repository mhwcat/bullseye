#include "camera.h"
#include "app_settings.h"
#include "math_utils.h"
#include "clogger.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>
#include <SDL.h>

namespace bullseye::camera {
    Camera::Camera(uint32_t screen_width, uint32_t screen_height) {
        position = glm::vec3(-10.f, 0.f, 4.f);
        previous_position = glm::vec3(0.f, 0.f, 0.f);

        up = glm::vec3(0.0f, 1.0f, 0.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        right = glm::vec3(1.0f, 0.0f, 0.0f);
        world_up = glm::vec3(0.0f, 1.0f, 0.0f);
        yaw = 0.f;
        pitch = 0.f;

        movement_speed = DEFAULT_MOVEMENT_SPEED;
        movement_direction = NONE;
        sensitivity = 0.1f;

        mouse_attached = false;
        free_fly = true;

        update_aspect_ratio(screen_width, screen_height);
        update_camera_vectors();
    }

    void Camera::update_aspect_ratio(uint32_t screen_width, uint32_t screen_height) {
        this->aspect_ratio = (float) screen_width / (float) screen_height;
    }

    void Camera::update_movement_speed(float movement_speed_delta) {
        if (this->movement_speed + movement_speed_delta < 0.0f) {
            this->movement_speed = 0.0f;
        } else {
            this->movement_speed += movement_speed_delta;
        }
    }

    void Camera::update_camera_vectors() {
        glm::vec3 new_front = glm::normalize(glm::vec3(
            cos(math_utils::to_radians(yaw)) * cos(math_utils::to_radians(pitch)),
            sin(math_utils::to_radians(pitch)),
            sin(math_utils::to_radians(yaw)) * cos(math_utils::to_radians(pitch))
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

    // @TODO: Fix weird jump happening above 90/-90 pitch in free-fly
    void Camera::process_mouse_input(float x_offset, float y_offset) {
        float x_offset_adjusted = x_offset * sensitivity;
        float y_offset_adjusted = y_offset * sensitivity;

        this->yaw += x_offset_adjusted;
        this->pitch += y_offset_adjusted;

        // Constraint pitch for first-person camera
        if (!this->free_fly) {
            if (this->pitch > 89.0f) {
                this->pitch = 89.0f;
            } else if (this->pitch < -89.0f) {
                this->pitch = -89.0f;
            }
        }

        update_camera_vectors();
    }

    void Camera::process_input(MovementDirection movement_direction) {
        this->movement_direction = movement_direction;
    }

    void Camera::update(float delta_time) {
        float velocity = movement_speed * delta_time;

        previous_position.x = position.x;
        previous_position.y = position.y;
        previous_position.z = position.z;

        switch (this->movement_direction) {
            case UP:
                if (this->free_fly) {
                    position += up * velocity;
                }
                break;
            case DOWN:
                if (this->free_fly) {
                    position -= up * velocity;
                }
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
            case FRONT_LEFT:
                position += (-right + front) * velocity;
                break;
            case FRONT_RIGHT:
                position += (right + front) * velocity;
                break;
            case BACK_LEFT:
                position -= (-right + front) * velocity;
                break;
            case BACK_RIGHT:
                position -= (right + front) * velocity;
                break;
            default:
                break;
        }

        // Keep camera (player) on the ground for first-person camera
        if (!this->free_fly) {
            position.y = 0.f;
        }
    }

    glm::mat4 Camera::get_perspective_matrix() {
        return glm::perspective(FOV, aspect_ratio, Z_NEAR, Z_FAR);
    }

    glm::mat4 Camera::get_view_matrix(float interp) {
        // TODO: Find lerp in glm and use it
        glm::vec3 interpolated_pos = math_utils::lerp(previous_position, position, interp);

        return glm::lookAt(interpolated_pos, (interpolated_pos + front), up);
    }

    glm::mat4 Camera::get_skybox_matrix() {
        glm::mat4 per = glm::perspective(FOV, aspect_ratio, Z_NEAR, Z_FAR);
        glm::mat4 view = glm::lookAt(glm::vec3(2.f, 0.f, -5.f), this->front, this->up);

       return per * view;
    }

    const glm::vec3* Camera::get_position() {
        return &this->position;
    }

    const float Camera::get_pitch() {
        return this->pitch;
    }

    const float Camera::get_yaw() {
        return this->yaw;
    }

    const bool Camera::is_mouse_attached() {
        return this->mouse_attached;
    }

    const glm::vec3& Camera::get_front() {
        return this->front;
    }

    void Camera::update_settings(app_settings::AppSettings *app_settings) {
        if (this->mouse_attached && !app_settings->camera_mouse_attached) {
            this->mouse_attached = false;
            SDL_SetRelativeMouseMode(SDL_FALSE);

            CLOG_DEBUG("Camera detached from mouse");
        } else if (!this->mouse_attached && app_settings->camera_mouse_attached) {
            this->mouse_attached = true;
            SDL_SetRelativeMouseMode(SDL_TRUE);

            CLOG_DEBUG("Camera attached to mouse");
        }

        this->free_fly = app_settings->camera_free_fly;
    }

}