#include "core/text_manager.hpp"
#include "core/resource_manager.hpp"
#include "core/status_handler.hpp"
#include "freetype/freetype.h"
#include <glad/glad.h>
#include <string>

std::map<std::string, TextManager::FontHandle> TextManager::fonts;
TextManager::FontHandle*                       TextManager::active_font;
FT_Library                                     TextManager::FT;

Text::Text(): ID(), x(0.0f), y(0.0f), w(0), h(0) {}

Text::Text(Text&& other) noexcept: ID(other.ID.ID), x(other.x), y(other.y), w(other.w), h(other.h) {
    other.ID.ID = 0;
}

Text::Text(unsigned int _ID, float _x, float _y, unsigned int _w, unsigned int _h): ID(_ID), x(_x), y(_y), w(_w), h(_h) {}

void TextManager::initialize()
{
    FT_Init_FreeType(&FT);
}

void TextManager::clear()
{
    fonts.clear();
    FT_Done_FreeType(FT);
}

void TextManager::load_font(const char* key, const char* file_path)
{
    if (!fonts.try_emplace(key, file_path).second)
    {
        std::string message = "Font under key \"";
        StatusHandler::log(WARNING, message + key + "\" already exists");
    }
}

void TextManager::set_font(const char* key)
{
    auto it = fonts.find(key);

    if (it == fonts.end())
    {
        std::string message = "Failed to find font under key \"";
        StatusHandler::log(WARNING, message + key + '\"');

        return;
    }

    active_font = &it->second;
}

Text& TextManager::create_text(const char16_t* text, Size fs, unsigned int& dip) {
    const FontHandle::BitmapData* bitmap;
    //unsigned int max_width  = 0;
    //unsigned int max_height = 0;

    //unsigned int row_w = 0;
    unsigned int width = 0;
    unsigned int above = 0;
    unsigned int below = 0;

    // Calculating for total width, max height above and below y = 0 line
    for (const char16_t* start = text; *start != u'\0'; ++start)
    {
        const char16_t ch = *start;
        bitmap            = active_font->get_char(ch, fs);

        if (bitmap == nullptr) {
            bitmap = active_font->get_char(u' ', fs);
        }

        width += bitmap->advance_x;
        above  = (above > bitmap->bearing_y) ? above : bitmap->bearing_y;

        if (bitmap->bearing_y <= bitmap->height) {
            below = (below > bitmap->height - bitmap->bearing_y) ? below : bitmap->height - bitmap->bearing_y;
        }
    }

    width -= bitmap->advance_x;
    width += bitmap->bearing_x + bitmap->width;
    dip    = below;

    // Generating a texture object for the whole text generated in OpenGL
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, above + below, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    unsigned int offset = 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (const char16_t* start = text; *start != u'\0'; ++start) {
        const char16_t ch = *start;
        bitmap            = active_font->get_char(ch, fs);

        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        offset + bitmap->bearing_x, below + (bitmap->bearing_y - bitmap->height),
                        bitmap->width, bitmap->height,
                        GL_RED, GL_UNSIGNED_BYTE, bitmap->data);

        offset += bitmap->advance_x;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture( GL_TEXTURE_2D, 0);

    ResourceManager::objects.entries.emplace_back(Text{texture, 0.0f, 0.0f, width, above + below});
    ResourceManager::objects.models.emplace_back();

    return std::get<Text>(ResourceManager::objects.entries[ResourceManager::objects.entries.size() - 1]);
}

Text& TextManager::create_text(const char16_t* text, Size fs, unsigned int& dip, unsigned int max_width) {
    const FontHandle::BitmapData* bitmap;
    unsigned int above = 0;
    unsigned int below = 0;

    // Calculating for total width, max height above and below y = 0 line
    for (const char16_t* start = text; *start != u'\0'; ++start)
    {
        const char16_t ch = *start;
        bitmap            = active_font->get_char(ch, fs);

        if (bitmap == nullptr) {
            bitmap = active_font->get_char(u' ', fs);
        }

        above  = (above > bitmap->bearing_y) ? above : bitmap->bearing_y;

        if (bitmap->bearing_y <= bitmap->height) {
            below = (below > bitmap->height - bitmap->bearing_y) ? below : bitmap->height - bitmap->bearing_y;
        }
    }

    dip = below;

    // Generating a texture object for the whole text generated in OpenGL
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, max_width, above + below, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    unsigned int offset = 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (const char16_t* start = text; *start != u'\0'; ++start) {
        const char16_t ch = *start;
        bitmap            = active_font->get_char(ch, fs);

        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        offset + bitmap->bearing_x, below + (bitmap->bearing_y - bitmap->height),
                        bitmap->width, bitmap->height,
                        GL_RED, GL_UNSIGNED_BYTE, bitmap->data);

        offset += bitmap->advance_x;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture( GL_TEXTURE_2D, 0);

    ResourceManager::objects.entries.emplace_back(Text{texture, 0.0f, 0.0f, max_width, above + below});
    ResourceManager::objects.models.emplace_back();

    return std::get<Text>(ResourceManager::objects.entries[ResourceManager::objects.entries.size() - 1]);
}

TextManager::FontHandle::FontHandle(const char* file_path): face(nullptr) {
    FT_Error error = FT_New_Face(FT, file_path, 0, &face);

    if (error) {
        std::string message = "Failed to load font stored at \"";
        StatusHandler::log(ERROR, message + file_path + "\": " + FT_Error_String(error));

        return;
    }

    for (int s = 0; s < 4; ++s) {
        Size size = static_cast<Size>(s);
        unsigned int size_px;

        switch (size) {
            case LARGE:
                size_px = 96;
                break;
            case MEDIUM:
                size_px = 48;
                break;
            case SMALL:
                size_px = 24;
                break;
            case TINY:
                size_px = 12;
                break;
        }

        FT_Set_Pixel_Sizes(face, 0, size_px);

        // Loading glyphs of ASCII chars
        for (char16_t ch = 32; ch <= 127; ++ch) {
            set_char(ch, size);
        }
    }
}

TextManager::FontHandle::~FontHandle() {
    for (auto& ch : chars) {
        ch.clear();
    }
    FT_Done_Face(face);
}

void TextManager::FontHandle::set_char(char16_t ch, Size font_size) {
    unsigned int size_px;
    switch (font_size) {
        case LARGE:
            size_px = 96;
            break;
        case MEDIUM:
            size_px = 48;
            break;
        case SMALL:
            size_px = 24;
            break;
        case TINY:
            size_px = 12;
            break;
    }

    FT_Set_Pixel_Sizes(face, 0, size_px);

    FT_Error error = FT_Load_Char(face, ch, FT_LOAD_RENDER);

    if (error) {
        std::u16string message = u"Failed to load glyph of char '";
        StatusHandler::log(ERROR, message + ch + u"': " + (const char16_t*)FT_Error_String(error));

        return;
    }

    FT_GlyphSlot slot = face->glyph;

    chars[font_size].try_emplace(ch,
                      slot->bitmap.buffer, slot->advance.x >> 6,
                      slot->bitmap.width, slot->bitmap.rows,
                      slot->bitmap_left, slot->bitmap_top);
};

const TextManager::FontHandle::BitmapData* TextManager::FontHandle::get_char(char16_t ch, Size font_size) {
    auto it = chars[font_size].find(ch);

    if (it == chars[font_size].end()) {
        set_char(ch, font_size);

        it = chars[font_size].find(ch);

        if (it == chars[font_size].end()) {
            return nullptr;
        }

        return &it->second;
    }

    return &it->second;
}

TextManager::FontHandle::BitmapData::BitmapData(unsigned char* src, long _advance_x,
                                                unsigned int _width, unsigned int _height,
                                                int _bearing_x, int _bearing_y):
    data(nullptr), advance_x(_advance_x), width(_width), height(_height), bearing_x(_bearing_x), bearing_y(_bearing_y)
{
    // Caching the glyph data and vertically inverting it
    data = new unsigned char[width * height];

    for (int r = 0; r < height; ++r) {
        memcpy(data + r * width,
               src + (height - r - 1) * width,
               width);
    }
}

TextManager::FontHandle::BitmapData::~BitmapData() {
    delete[] data;
}
