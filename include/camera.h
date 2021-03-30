#ifndef BULLSEYE_CAMERA_H
#define BULLSEYE_CAMERA_H

#include "structs.h"
#include <glm/glm.hpp>

using namespace bullseye::structs;

namespace bullseye::camera {
    const float FOV                         = 45.f;
    const float Z_NEAR                      = 0.1f;
    const float Z_FAR                       = 1000.f;
    const float DEFAULT_MOVEMENT_SPEED      = 10.f;

    enum MovementDirection {
        UP, DOWN, FRONT, BACK, LEFT, RIGHT, NONE
    };

    class Camera {
        public:
            Camera(uint32_t screen_width, uint32_t screen_height);

            void update_camera_vectors();
            void update_movement_speed(float movement_speed_delta);
            void update_aspect_ratio(uint32_t screen_width, uint32_t screen_height);
            void update(float delta_time);
            void process_mouse_input(float x_offset, float y_offset);
            void process_input(MovementDirection _movement_direction);

            glm::mat4 get_perspective_matrix();
            glm::mat4 get_view_matrix(float interp);

            const glm::vec3* get_position();
            const bool is_mouse_attached();
            void switch_mouse_attached();
        private:
            glm::vec3 position;
            glm::vec3 previous_position;
            float aspect_ratio;

            float movement_speed;
            MovementDirection movement_direction;
            float sensitivity;
            float zoom;

            glm::vec3 up;
            glm::vec3 front;
            glm::vec3 right;
            glm::vec3 world_up;
            float yaw;
            float pitch;

            bool mouse_attached;
    };
}

#endif