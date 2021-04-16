#ifndef BULLSEYE_TEXTURE_MANAGER_H
#define BULLSEYE_TEXTURE_MANAGER_H

#include <string>
#include <unordered_map>

namespace bullseye::texture {
    static const uint32_t TEXTURE_MANAGER_INITIAL_SIZE = 64;

    struct Texture {
        uint32_t id;
    };

    class TextureManager {
        public:
            TextureManager();

            void load_texture(const std::string name, const std::string& path);
            void use_texture(const std::string& name, const uint32_t shader_id);
            void unload_texture(std::string& name);
            void unload();
        private:
            std::unordered_map<std::string, Texture*> textures;
    };
}

#endif