#ifndef BULLSEYE_MESH_H
#define BULLSEYE_MESH_H

#include <string>
#include <vector>
#include "glm/glm.hpp"

#include "shader.h"

namespace bullseye::mesh {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texture_coords;
    };

    struct Texture {
        uint32_t id;
        std::string type;
    };

    class Mesh {
        private:
            std::string name;
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::vector<Texture> textures;
            uint32_t vao;
            uint32_t vbo;
            glm::vec3 scale;
            glm::vec3 extents;

            void load_obj_file(std::string path);
            void load_and_setup_vertices(const float* vertices, uint32_t vertices_len);
            void setup_mesh();
            void calculate_bounding_box();
            
        public:
            Mesh(std::string name, std::string path, glm::vec3 scale = glm::vec3(1.f));
            Mesh(std::string name, const float* vertices, uint32_t vertices_len);
            void draw(shader::Shader& shader);
            void draw_light_cube();
            void unload();
            const char* get_name();
            void rescale(glm::vec3 scale);
            const glm::vec3& get_extents();
    };
}

#endif