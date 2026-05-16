#pragma once

#include "core/handles/texture.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>

enum Size {
    LARGE = 0,
    MEDIUM,
    SMALL,
    TINY
};

struct Text {
    Text();
    Text(Text&& other) noexcept;
    Text& operator=(Text&& other) noexcept;
    Text(unsigned int _ID, float _x, float _y, unsigned int _w, unsigned int _h);

    Text(const Text& other) = delete;
    Text& operator=(const Text& other) = delete;

    Texture      ID;
    float        x;
    float        y;
    unsigned int w;
    unsigned int h;
};

class TextManager {
public:
    static void  initialize();
    static void  clear();

    static void  load_font(const char* key, const char* file_path);
    static void  set_font(const char* key);
    static Text& create_text(const char16_t* text, Size fs, unsigned int& dip);
    static Text& create_text(const char16_t* text, Size fs, unsigned int& dip, unsigned int max_width);

private:
    TextManager() = delete;
    TextManager(const TextManager& other) = delete;
    TextManager(TextManager&& other) = delete;
    TextManager& operator=(const TextManager& other) = delete;
    TextManager& operator=(TextManager&& other) = delete;
    ~TextManager() = delete;

    struct FontHandle {
    public:
        FontHandle(const char* file_path);
        ~FontHandle();

        struct BitmapData {
            BitmapData(unsigned char* _data, long _advance_x,
                       unsigned int _width, unsigned int _height,
                       int _bearing_x, int _bearing_y);
            ~BitmapData();

            unsigned char* data;
            unsigned int   width;
            unsigned int   height;
            int            bearing_x;
            int            bearing_y;
            long           advance_x;
        };

        void set_char(char16_t ch, Size font_size);
        const BitmapData* get_char(char16_t ch, Size font_size);

    private:
        std::map<char16_t, BitmapData> chars[4];
        FT_Face                        face;
    };

    static std::map<std::string, FontHandle> fonts;
    static FontHandle*                       active_font;
    static FT_Library                        FT;
};
