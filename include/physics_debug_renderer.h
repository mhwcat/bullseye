#ifndef BULLSEYE_PHYSICS_DEBUG_RENDERER_H
#define BULLSEYE_PHYSICS_DEBUG_RENDERER_H

#include <stdint.h>
#include "reactphysics3d/reactphysics3d.h"
#include "shader.h"
#include "camera.h"
#include "app_settings.h"

namespace bullseye::render {
    class PhysicsDebugRenderer {
        public:
            PhysicsDebugRenderer(rp3d::PhysicsWorld* physics_world);
            ~PhysicsDebugRenderer();

            void draw(shader::Shader &shader, camera::Camera &camera, float interp);
            void update_settings(app_settings::AppSettings* app_settings);
        private:
            rp3d::PhysicsWorld* physics_world;

            // Debug lines
            uint32_t lines_vao;
            uint32_t lines_vbo;
            // Debug vertices
            uint32_t triangles_vao;
            uint32_t triangles_vbo;

            void init();
            void update_vbo_data(const rp3d::DebugRenderer& physics_debug_renderer, const uint32_t lines_count, const uint32_t triangles_count);

    };
}

#endif