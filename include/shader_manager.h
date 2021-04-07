#ifndef BULLSEYE_SHADER_MANAGER_H
#define BULLSEYE_SHADER_MANAGER_H

#include <string>
#include <unordered_map>

#include "shader.h"

namespace bullseye::shader {
    static const uint32_t SHADER_MANAGER_INITIAL_SIZE = 32;

    class ShaderManager {
        public:
            ShaderManager();
            ~ShaderManager();
            void load_shader(std::string name, std::string vertex_shader_path, std::string fragment_shader_path);
            void unload_shader(const std::string &name);
            void unload();

            Shader& get_shader(const std::string &name);
            void use_shader(const std::string &name);
            void set_vec2(const std::string &name, const std::string &uniform_name, const glm::vec2 &value);
            void set_vec3(const std::string &name, const std::string &uniform_name, const glm::vec3 &value);
            void set_mat4(const std::string &name, const std::string &uniform_name, const glm::mat4 &value);
        private:
            std::unordered_map<std::string, Shader*> shaders;
    };
}

#endif