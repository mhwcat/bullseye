#ifndef BULLSEYE_MESH_MANAGER_H
#define BULLSEYE_MESH_MANAGER_H

#include <string>
#include <unordered_map>

#include "mesh.h"

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace bullseye::mesh {
    static const uint32_t MESH_MANAGER_INITIAL_SIZE = 128;

    class MeshManager {
        public:
            MeshManager();
            ~MeshManager();
            void load_mesh(std::string name, std::string mesh_file_path, glm::vec3 scale = glm::vec3(1.f));
            void unload_mesh(const std::string &name);
            void unload();

            mesh::Mesh* get_mesh(const std::string &name);
            void draw_mesh(const std::string &name);

        private:
            std::unordered_map<std::string, Mesh*> meshes;
    };
}

#endif