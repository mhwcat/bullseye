#include "shader.h"
#include "logger.h"

#include <fstream>
#include <sstream>
#include "glad/glad.h"

using namespace bullseye;

namespace bullseye::shader {
    Shader::Shader(std::string _name) {
        name = _name;
    }

    inline std::string Shader::load_file(const char* path) {
        std::ifstream input_file(path);
        if (input_file) {
            std::stringstream buffer;
            buffer << input_file.rdbuf();

            return buffer.str();
        }
        else {
            logger::error("Failed loading shader file %s!", path);

            return std::string();
        }
    }

    void Shader::load_vertex_shader(const char* path) {
        logger::debug("Loading vertex shader [name=%s, path=%s]", this->name.c_str(), path);

        std::string shader_src = load_file(path);
        const char* v_shader_src = shader_src.c_str();

        this->vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(this->vertex_shader_id, 1, &v_shader_src, NULL);
        glCompileShader(this->vertex_shader_id);

        int success;
        char info_log[512];
        glGetShaderiv(this->vertex_shader_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(this->vertex_shader_id, 512, NULL, info_log);
            logger::error("Failed to compile vertex shader: %s [name=%s]", info_log, this->name.c_str());
        }
    }

    void Shader::load_fragment_shader(const char* path) {
        logger::debug("Loading fragment shader [name=%s, path=%s]", this->name.c_str(), path);

        std::string shader_src = load_file(path);

        const char* f_shader_src = shader_src.c_str();

        this->fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(this->fragment_shader_id, 1, &f_shader_src, NULL);
        glCompileShader(this->fragment_shader_id);

        int success;
        char info_log[512];
        glGetShaderiv(this->fragment_shader_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(this->fragment_shader_id, 512, NULL, info_log);
            logger::error("Failed to compile fragment shader: %s [name=%s]", info_log, this->name.c_str());
        }
    }

    void Shader::link_shaders() {
        logger::debug("Linking shaders [name=%s]", this->name.c_str());

        this->id = glCreateProgram();
        glAttachShader(this->id, this->vertex_shader_id);
        glAttachShader(this->id, this->fragment_shader_id);
        glLinkProgram(this->id);

        int success;
        char info_log[512];
        glGetProgramiv(this->id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(this->id, 512, NULL, info_log);
            logger::error("Failed to link shader program: %s [name=%s]", info_log, this->name.c_str());
        }

        glDeleteShader(this->vertex_shader_id);
        glDeleteShader(this->fragment_shader_id);
    }

    const char* Shader::get_vertex_shader_src() {
        return vertex_shader_src.c_str();
    }

    const char* Shader::get_fragment_shader_src() {
        return fragment_shader_src.c_str();
    }

    void Shader::use() {
        glUseProgram(this->id);
    }
    
    void Shader::delete_program() {
        logger::debug("Deleting shader program [name=%s]", this->name.c_str());

        glDeleteProgram(this->id);
    }

    void Shader::set_bool(const std::string &name, bool value) {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int) value);
    }

    void Shader::set_int(const std::string &name, int value) {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
    }

    void Shader::set_float(const std::string &name, float value) {
        glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
    }

    void Shader::set_vec2(const std::string &name, const glm::vec2 &value) {
        glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec3(const std::string &name, const glm::vec3 &value) {
        glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
    }

    void Shader::set_mat4(const std::string &name, const glm::mat4 &value) {
        glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, false, &value[0][0]);
    }
}

