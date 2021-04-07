#include "hud.h"
#include "shader.h"
#include "skybox.h"

#include <glad/glad/glad.h>

namespace bullseye::hud {
    Hud::Hud() {
        init();
    }

    void Hud::init() {
        uint32_t vbo;
        
        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox::SKYBOX_VERTICES), &skybox::SKYBOX_VERTICES, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    }

    void Hud::draw() {
        draw_crosshair();
    }

    void Hud::draw_crosshair() {
        //glDepthFunc(GL_LEQUAL);
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        //glDepthFunc(GL_LESS);
    }
}