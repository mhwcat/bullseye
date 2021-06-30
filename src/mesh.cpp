#include "mesh.h"

#include <string>
#include <vector>
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "tobjl/tiny_obj_loader.h"

#include "logger.h"

namespace bullseye::mesh {
    Mesh::Mesh(std::string name, std::string path, glm::vec3 scale) {
        this->name = name;
        this->scale = scale;

        load_obj_file(path);
        setup_mesh();
        calculate_bounding_box();
    }

    Mesh::Mesh(std::string name, const float* vertices, uint32_t vertices_len) {
        this->name = name;
        this->scale = glm::vec3(1.f);

        load_and_setup_vertices(vertices, vertices_len);
    }

    void Mesh::load_and_setup_vertices(const float* vertices, uint32_t vertices_len) {
        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &this->vbo);

        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices_len * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

        glBindVertexArray(0);
    }

    void Mesh::load_obj_file(std::string path) {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "assets/models/";

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config)) {
            if (!reader.Error().empty()) {
                logger::error("TinyObjReader error: %s", reader.Error().c_str());
            }
        }

        if (!reader.Warning().empty()) {
            logger::warn("TinyObjReader warning: %s", reader.Warning().c_str());
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                mesh::Vertex vertex{};
                vertex.position = glm::vec3(attrib.vertices[3 * index.vertex_index + 0] * this->scale.x, 
                    attrib.vertices[3 * index.vertex_index + 1] * this->scale.y, 
                    -attrib.vertices[3 * index.vertex_index + 2] * this->scale.z);
                if (index.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3 * index.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * index.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * index.normal_index + 2];

                    vertex.normal = glm::vec3(nx, ny, nz);
                }
                if (index.texcoord_index >= 0) {
                    vertex.texture_coords = glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0], attrib.texcoords[2 * index.texcoord_index + 1]);
                } else {
                    vertex.texture_coords = glm::vec2(0.f);
                }

                this->vertices.push_back(vertex);
                this->indices.push_back(indices.size());
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
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture_coords));
        
        glBindVertexArray(0); 
    }

    void Mesh::calculate_bounding_box() {
        float max_x, max_y, max_z;
        max_x = this->vertices[0].position.x;
        max_y = this->vertices[0].position.y;
        max_z = this->vertices[0].position.z;

        for (auto &vert : this->vertices) {
            if (vert.position.x > max_x) {
                max_x = vert.position.x;
            }
            if (vert.position.y > max_y) {
                max_y = vert.position.y;
            }
            if (vert.position.z > max_z) {
                max_z = vert.position.z;
            }                        
        }

        logger::debug("Calculated bounding box for Mesh %s, max extents [x=%.2f, y=%.2f, z=%.2f]", this->name.c_str(), max_x, max_y, max_z);

        this->extents = glm::vec3(max_x, max_y, max_z);
    }

    void Mesh::draw() {
        glBindVertexArray(this->vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::draw_light_cube() {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    void Mesh::unload() {
        logger::debug("Unloading mesh [name=%s]", this->name.c_str());

        glDeleteVertexArrays(1, &this->vao);
        glDeleteBuffers(1, &this->vbo);
    }

    const char* Mesh::get_name() {
        return this->name.c_str();
    }

    const glm::vec3& Mesh::get_extents() {
        return this->extents;
    }
}
