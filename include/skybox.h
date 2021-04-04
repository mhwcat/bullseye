#ifndef BULLSEYE_SKYBOX_H
#define BULLSEYE_SKYBOX_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "shader.h"

namespace bullseye::skybox {
    static const float SKYBOX_VERTICES[] = {
        -1.0f,  -1.0f,  1.0f,
        -1.0f,  1.0f,   1.0f,
        1.0f,   1.0f,   1.0f,
        1.0f,   1.0f,   1.0f,
        1.0f,   -1.0f,  1.0f,
        -1.0f,  -1.0f,  1.0f,
    };

    class Skybox {
        public:
            Skybox(std::vector<std::string> texture_paths, std::string vert_shader_path, std::string frag_shader_path);
            virtual ~Skybox();

            void draw(glm::mat4 projection, glm::mat4 view);
        private:
            shader::Shader *shader;
            uint32_t texture_id;
            uint32_t vao;

            void setup_skybox(std::vector<std::string> texture_paths);
    };
}

#endif