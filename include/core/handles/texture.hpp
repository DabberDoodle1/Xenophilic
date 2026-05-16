#pragma once

struct Texture {
    Texture();
    Texture(unsigned int other_ID);
    Texture(unsigned int width, unsigned int height, unsigned int i_format, unsigned int format, const unsigned char* data);
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();

    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;

    void bind() const;

    unsigned int ID;
};
