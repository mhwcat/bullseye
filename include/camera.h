#ifndef BULLSEYE_CAMERA_H
#define BULLSEYE_CAMERA_H

#include "app_settings.h"

#include "glm/glm.hpp"

namespace bullseye::camera {
    const float FOV                         = 45.f;
    const float Z_NEAR                      = 0.01f;
    const float Z_FAR                       = 1000.f;
    const float DEFAULT_MOVEMENT_SPEED      = 10.f;

    enum MovementDirection {
        UP, DOWN, FRONT, BACK, LEFT, RIGHT, NONE,
        FRONT_LEFT, FRONT_RIGHT, BACK_LEFT, BACK_RIGHT
    };

    class Camera {
        public:
            Camera(uint32_t screen_width, uint32_t screen_height);

            void update_camera_vectors();
            void update_movement_speed(float movement_speed_delta);
            void update_aspect_ratio(uint32_t screen_width, uint32_t screen_height);
            void update(float delta_time);
            void process_mouse_input(float x_offset, float y_offset);
            void process_input(MovementDirection movement_direction);

            glm::mat4 get_perspective_matrix();
            glm::mat4 get_view_matrix(float interp);
            glm::mat4 get_skybox_matrix();
            const glm::vec3* get_position();
            const float get_pitch();
            const float get_yaw();
            const bool is_mouse_attached();

            void update_settings(app_settings::AppSettings *app_settings);
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
            bool free_fly;
    };
}

#endif