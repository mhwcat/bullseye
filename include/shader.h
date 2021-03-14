#ifndef BULLSEYE_SHADER_H
#define BULLSEYE_SHADER_H

#include <string>

namespace bullseye::shader {
    class Shader {
    private:
        std::string name;
        std::string vertex_shader_src;
        std::string fragment_shader_src;

        inline std::string load_file(const char* path);

    public:
        Shader(std::string _name);
        void load_vertex_shader(const char* path);
        void load_fragment_shader(const char* path);
        const char* get_vertex_shader_src();
        const char* get_fragment_shader_src();
    };
}

#endif