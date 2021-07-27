#include "texture_manager.h"

#include <string>

#include "clogger.h"
#include "stb/stb_image.h"
#include "glad/glad.h"

namespace bullseye::texture {
    TextureManager::TextureManager() {
        this->textures.reserve(TEXTURE_MANAGER_INITIAL_SIZE);
    }

    void TextureManager::load_texture(const std::string name, const std::string& path) {
        Texture* texture = new Texture;

        glGenTextures(1, &texture->id);
        glBindTexture(GL_TEXTURE_2D, texture->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int x, y, n;
        unsigned char *data = stbi_load(path.c_str(), &x, &y, &n, 0);
        
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);

            CLOG_DEBUG("Loaded texture [path=%s, w=%d, h=%d]", path.c_str(), x, y);
        } else {
            CLOG_ERROR("Failed to load texture [path=%s]", path.c_str());
        }

        this->textures.insert({ name, texture });
    }

    void TextureManager::use_texture(const std::string& name, const uint32_t shader_id) {
        Texture* texture = this->textures.at(name);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader_id, "texture_diffuse1"), 0);
        glBindTexture(GL_TEXTURE_2D, texture->id);
    }

    void TextureManager::unload_texture(std::string& name) {

    }

    void TextureManager::unload() {

    }
}