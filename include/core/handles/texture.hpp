#pragma once

class Texture {
public:
    Texture(unsigned int width, unsigned int height, unsigned int i_format, unsigned int format, const unsigned char* data);
    Texture(unsigned int copy_ID);
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();

    void bind() const;
private:
    unsigned int ID;
};
