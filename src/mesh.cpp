#include "mesh.h"

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "tobjl/tiny_obj_loader.h"

#include "logger.h"

namespace bullseye::mesh {
    Mesh::Mesh(std::string path) {

        load_obj_file(path);
        setup_mesh();
    }

    void Mesh::load_obj_file(std::string path) {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "./";

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config)) {
            if (!reader.Error().empty()) {
                logger::error("TinyObjReader error: %s", reader.Error());
            }
            return;
        }

        if (!reader.Warning().empty()) {
            logger::warn("TinyObjReader warning: %s", reader.Warning());
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                mesh::Vertex vertex{};
                vertex.position = glm::vec3(attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], -attrib.vertices[3 * index.vertex_index + 2]);
                if (index.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3 * index.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * index.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * index.normal_index + 2];

                    vertex.normal = glm::vec3(nx, ny, nz);
                    vertex.texture_coords = glm::vec3(0); // @TODO: Implement texture coords
                }

                this->vertices.push_back(vertex);
                this->indices.push_back(indices.size()); // @KNOWHOW: why size()?
            }
        }

        logger::debug("Loaded %s mesh [vertices=%d, indices=%d]", path.c_str(), vertices.size(), indices.size());
    }

    void Mesh::setup_mesh() {
        uint32_t ebo;

        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &this->vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        // positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

        // tex coords
        // glEnableVertexAttribArray(2);
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture_coords));
        
        glBindVertexArray(0); 
    }

    void Mesh::draw(shader::Shader &shader) {
        // @TODO: Implement textures
        // uint32_t diffuse_num = 1;
        // uint32_t specular_num = 1;

        // for (uint32_t i = 0; i < textures.size(); i++) {
        //     glActive
        // }

        glBindVertexArray(this->vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::unload() {
        glDeleteVertexArrays(1, &this->vao);
        glDeleteBuffers(1, &this->vbo);
    }
}
