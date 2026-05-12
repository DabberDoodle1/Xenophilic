#include "core/handles/texture.hpp"
#include "core/status_handler.hpp"
#include <glad/glad.h>

Texture::Texture(unsigned int width, unsigned int height, unsigned int i_format, unsigned int p_format, const unsigned char* data) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, i_format, width, height, 0, p_format, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(unsigned int copy_ID): ID(copy_ID) {
    ;
}

Texture::Texture(Texture&& other) noexcept {
    if (ID != 0) {
        glDeleteTextures(1, &ID);
    }

    ID       = other.ID;
    other.ID = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (ID != 0) {
            glDeleteTextures(1, &ID);
        }

        ID       = other.ID;
        other.ID = 0;
    }

    return *this;
}

Texture::~Texture() {
    if (ID != 0) {
        glDeleteTextures(1, &ID);
    }
}

void Texture::bind() const {
    if (ID == 0) {
        StatusHandler::log(WARNING, "Texture is empty");
    }

    glBindTexture(GL_TEXTURE_2D, ID);
}
