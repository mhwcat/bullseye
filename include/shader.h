#ifndef BULLSEYE_SHADER_H
#define BULLSEYE_SHADER_H

#include <string>
#include <glm/glm.hpp>

namespace bullseye::shader {
    class Shader {
    private:
        unsigned int id;
        std::string name;
        std::string vertex_shader_src;
        std::string fragment_shader_src;
        uint32_t vertex_shader_id;
        uint32_t fragment_shader_id;

        inline std::string load_file(const char* path);

    public:
        Shader(std::string _name);
        void load_vertex_shader(const char* path);
        void load_fragment_shader(const char* path);
        const char* get_vertex_shader_src();
        const char* get_fragment_shader_src();
        void use();
        void link_shaders();
        void delete_program();
        void set_bool(const std::string &name, bool value);
        void set_int(const std::string &name, int value);
        void set_vec3(const std::string &name, const glm::vec3 &value);
        void set_mat4(const std::string &name, const glm::mat4 &value);
    };
}

#endif