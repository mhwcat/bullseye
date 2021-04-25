#include "physics_debug_renderer.h"
#include "shader.h"
#include "camera.h"
#include "logger.h"

#include "glad/glad.h"
#include "glm/gtc/matrix_inverse.hpp"

namespace bullseye::render {
    PhysicsDebugRenderer::PhysicsDebugRenderer(rp3d::PhysicsWorld* physics_world) {
        this->physics_world = physics_world;

        init();
    }

    PhysicsDebugRenderer::~PhysicsDebugRenderer() {

    }

    void PhysicsDebugRenderer::init() {
        rp3d::DebugRenderer& physics_debug_renderer = this->physics_world->getDebugRenderer();
        physics_debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
        physics_debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
        physics_debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
        physics_debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);

        glGenVertexArrays(1, &this->lines_vao);
        glGenVertexArrays(1, &this->triangles_vao);
        glGenBuffers(1, &this->lines_vbo);
        glGenBuffers(1, &this->triangles_vbo);
    }

    void PhysicsDebugRenderer::update_vbo_data(const rp3d::DebugRenderer& physics_debug_renderer, const uint32_t lines_count, const uint32_t triangles_count) {
        if (lines_count > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, this->lines_vbo);
            glBufferData(GL_ARRAY_BUFFER, lines_count * sizeof(rp3d::DebugRenderer::DebugLine), physics_debug_renderer.getLinesArray(), GL_STREAM_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        if (triangles_count > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, this->triangles_vbo);
            glBufferData(GL_ARRAY_BUFFER, triangles_count * sizeof(rp3d::DebugRenderer::DebugTriangle), physics_debug_renderer.getTrianglesArray(), GL_STREAM_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    
    void PhysicsDebugRenderer::draw(shader::Shader &shader, camera::Camera &camera, float interp) {
        const rp3d::DebugRenderer& physics_debug_renderer = this->physics_world->getDebugRenderer();
        const uint32_t lines_count = physics_debug_renderer.getNbLines();
        const uint32_t triangles_count = physics_debug_renderer.getNbTriangles();

        update_vbo_data(physics_debug_renderer, lines_count, triangles_count);

        shader.use();
        shader.set_mat4("model", glm::mat4(1.0f));
        shader.set_mat4("view", camera.get_view_matrix(interp));
        shader.set_mat4("projection", camera.get_perspective_matrix());

        // @TODO: Use layout() in shader and hardcode these locations
        const uint32_t vertex_position_location = shader.get_attrib_location("vertex_position");
        const uint32_t vertex_color_location = shader.get_attrib_location("vertex_color");

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (lines_count > 0) {
            glBindVertexArray(this->lines_vao);
            glBindBuffer(GL_ARRAY_BUFFER, this->lines_vbo);

            glEnableVertexAttribArray(vertex_position_location);
            glVertexAttribPointer(vertex_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(rp3d::Vector3) + sizeof(rp3d::uint32), (char *) nullptr);

            glEnableVertexAttribArray(vertex_color_location);
            glVertexAttribIPointer(vertex_color_location, 3, GL_UNSIGNED_INT, sizeof(rp3d::Vector3) + sizeof(rp3d::uint32), (void *) sizeof(rp3d::Vector3));

            glDrawArrays(GL_LINES, 0, lines_count * 2);

            glDisableVertexAttribArray(vertex_position_location);
            glDisableVertexAttribArray(vertex_color_location);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        if (triangles_count > 0) {
            glBindVertexArray(this->triangles_vao);
            glBindBuffer(GL_ARRAY_BUFFER, this->triangles_vbo);

            glEnableVertexAttribArray(vertex_position_location);
            glVertexAttribPointer(vertex_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(rp3d::Vector3) + sizeof(rp3d::uint32), (char *) nullptr);

            glEnableVertexAttribArray(vertex_color_location);
            glVertexAttribIPointer(vertex_color_location, 3, GL_UNSIGNED_INT, sizeof(rp3d::Vector3) + sizeof(rp3d::uint32), (void *) sizeof(rp3d::Vector3));

            glDrawArrays(GL_TRIANGLES, 0, triangles_count * 3);

            glDisableVertexAttribArray(vertex_position_location);
            glDisableVertexAttribArray(vertex_color_location);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void PhysicsDebugRenderer::update_settings(app_settings::AppSettings* app_settings) {
        this->physics_world->setIsDebugRenderingEnabled(app_settings->physics_debug_draw);
    }

}