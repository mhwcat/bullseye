#include "shader_manager.h"
#include "logger.h"

#include <string>
#include <stdexcept>

namespace bullseye::shader {
    ShaderManager::ShaderManager() {
        this->shaders.reserve(SHADER_MANAGER_INITIAL_SIZE);
    }

    ShaderManager::~ShaderManager() {

    }

    void ShaderManager::load_shader(std::string name, std::string vertex_shader_path, std::string fragment_shader_path) {
        Shader *shader = new Shader(name);
        shader->load_vertex_shader(vertex_shader_path.c_str());
        shader->load_fragment_shader(fragment_shader_path.c_str());
        shader->link_shaders();

        this->shaders.insert({ name, shader });
    }

    void ShaderManager::unload_shader(const std::string &name) {
        if (this->shaders.find(name) != this->shaders.end()) {
            Shader *shader_to_unload = this->shaders.at(name);
            shader_to_unload->delete_program();

            delete shader_to_unload;
            this->shaders.erase(name);
        } else {
            logger::error("Cannot unload shader %s, shader not present in ShaderManager!", name.c_str());
        }
    }

    void ShaderManager::unload() {
        for (auto &shader : this->shaders) {
            shader.second->delete_program();
            delete shader.second;
        }

        this->shaders.clear();
    }

    void ShaderManager::use_shader(const std::string &name) {
        try {
            this->shaders.at(name)->use();
        } catch (const std::out_of_range& e) {
            logger::error("Shader %s not present in ShaderManager! [error=%s]", name.c_str(), e.what());
        }
    }

    Shader& ShaderManager::get_shader(const std::string &name) {
        return *this->shaders.at(name);
    }

    void ShaderManager::set_vec3(const std::string &name, const std::string &uniform_name, const glm::vec3 &value) {
        this->shaders.at(name)->set_vec3(uniform_name, value);
    }

    void ShaderManager::set_mat4(const std::string &name, const std::string &uniform_name, const glm::mat4 &value) {
        this->shaders.at(name)->set_mat4(uniform_name, value);
    }

    void ShaderManager::set_float(const std::string &name, const std::string &uniform_name, const float value) {
        this->shaders.at(name)->set_float(uniform_name, value);
    }
}