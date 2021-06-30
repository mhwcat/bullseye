#include "mesh_manager.h"
#include "logger.h"

#include <string>
#include <stdexcept>

namespace bullseye::mesh {
    MeshManager::MeshManager() {

    }

    MeshManager::~MeshManager() {

    }

    void MeshManager::load_mesh(std::string name, std::string mesh_file_path, glm::vec3 scale) {
        Mesh* mesh = new Mesh(name, mesh_file_path, scale);

        this->meshes.insert({ name, mesh });

        logger::debug("Mesh loaded from file [name=%s, file=%s]", name.c_str(), mesh_file_path.c_str());
    }

    mesh::Mesh* MeshManager::get_mesh(const std::string &name) {
        return this->meshes.at(name);
    }

    void MeshManager::draw_mesh(const std::string &name) {
        this->meshes.at(name)->draw();
    }

    void MeshManager::unload_mesh(const std::string &name) {
        if (this->meshes.find(name) != this->meshes.end()) {
            Mesh *mesh_to_unload = this->meshes.at(name);
            mesh_to_unload->unload();

            delete mesh_to_unload;
            this->meshes.erase(name);
        } else {
            logger::error("Cannot unload mesh, mesh not present in MeshManager! [name=%s]", name.c_str());
        }       

        logger::debug("Mesh unloaded [name=%s]", name.c_str()); 
    }

    void MeshManager::unload() {
        for (auto &mesh : this->meshes) {
            mesh.second->unload();
            delete mesh.second;
        }

        this->meshes.clear();
    }
}


