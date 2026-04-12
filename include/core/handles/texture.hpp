#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

// Texture handle wrapper for automatic deallocation of texture memory

class Texture {
public:
    Texture(unsigned int width, unsigned int height, unsigned int i_format, unsigned int format, const unsigned char* data);
    Texture(unsigned int copy_ID);
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) = delete;
    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) = delete;
    ~Texture();

    void bind() const;
private:
    unsigned int ID;
};

#endif
