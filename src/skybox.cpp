#include "skybox.h"

#include <vector>
#include <string>
#include <stb/stb_image.h>
#include "glad/glad.h"

#include "logger.h"
#include "shader.h"

namespace bullseye::skybox {
    Skybox::Skybox(std::vector<std::string> texture_paths, std::string vert_shader_path, std::string frag_shader_path) {
        this->shader = new shader::Shader("skybox");
        this->shader->load_vertex_shader(vert_shader_path.c_str());
        this->shader->load_fragment_shader(frag_shader_path.c_str());
        this->shader->link_shaders();

        this->setup_skybox(texture_paths);
    }

    Skybox::~Skybox() {
    }

    void Skybox::setup_skybox(std::vector<std::string> texture_paths) {
        glGenTextures(1, &this->texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture_id);

        int x, y, n;
        uint32_t i = 0;
        for (auto texture_path : texture_paths) {
            unsigned char *data = stbi_load(texture_path.c_str(), &x, &y, &n, 0);
            
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);

                logger::debug("Loaded texture [path=%s, w=%d, h=%d]", texture_path.c_str(), x, y);
            } else {
                logger::error("Failed to load texture [path=%s]", texture_path.c_str());
            }

            i++;
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        uint32_t vbo;
        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox::SKYBOX_VERTICES), &skybox::SKYBOX_VERTICES, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    }

    void Skybox::draw(glm::mat4 projection, glm::mat4 view) {
        glDepthFunc(GL_LEQUAL);
        this->shader->use();
        this->shader->set_mat4("proj", projection);
        this->shader->set_mat4("view", view);

        glBindVertexArray(this->vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

    void Skybox::unload() {
        logger::debug("Unloading skybox shader");

        if (shader != nullptr) {
            shader->delete_program();
            delete shader;
        }
    }
}