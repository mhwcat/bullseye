#ifndef BULLSEYE_MESH_H
#define BULLSEYE_MESH_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

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
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::vector<Texture> textures;
            uint32_t vao;

            void load_obj_file(std::string path);
            void setup_mesh();
            
        public:
            Mesh(std::string path);
            void draw(shader::Shader& shader);
    };
}

#endif