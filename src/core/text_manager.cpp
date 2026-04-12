#include "core/text_manager.hpp"
#include "core/status_handler.hpp"
#include "freetype/freetype.h"
#include <glad/glad.h>

std::map<std::string, TextManager::FontHandle> TextManager::fonts;
TextManager::FontHandle* TextManager::active_font;
FT_Library TextManager::FT;

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

unsigned int TextManager::create_text(const char16_t* text, unsigned int& _width, unsigned int& _height)
{
    const FontHandle::BitmapData* bitmap;
    unsigned int width  = 0;
    unsigned int height = 0;

    for (const char16_t* start = text; *start != u'\0'; ++start)
    {
        const char16_t ch = *start;
        bitmap = active_font->get_char(ch);

        width += bitmap->advance_x;
        height = height > bitmap->height ? height : bitmap->height;
    }

    width -= bitmap->advance_x;
    width += bitmap->width + bitmap->bearing_x;

    _width  = width;
    _height = height;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    unsigned int offset = 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (const char16_t* start = text; *start != u'\0'; ++start)
    {
        const char16_t ch = *start;
        bitmap = active_font->get_char(ch);

        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        offset + bitmap->bearing_x, height - bitmap->bearing_y,
                        bitmap->width, bitmap->height,
                        GL_RED, GL_UNSIGNED_BYTE, bitmap->data);

        offset += bitmap->advance_x;;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glBindTexture( GL_TEXTURE_2D, 0);
    return texture;
}

TextManager::FontHandle::FontHandle(const char* file_path): face(nullptr)
{
    FT_Error error = FT_New_Face(FT, file_path, 0, &face);

    if (error)
    {
        std::string message = "Failed to load font stored at \"";
        StatusHandler::log(ERROR, message + file_path + "\": " + FT_Error_String(error));

        return;
    }

    FT_Set_Pixel_Sizes(face, 48, 48);

    // Loading glyphs of ASCII chars
    for (char16_t ch = 32; ch <= 127; ++ch)
        set_char(ch);
}

TextManager::FontHandle::~FontHandle()
{
    chars.clear();
    FT_Done_Face(face);
}

void TextManager::FontHandle::set_char(char16_t ch)
{
    FT_Error error = FT_Load_Glyph(face,
                  FT_Get_Char_Index(face, ch),
                  FT_LOAD_RENDER);

    if (error)
    {
        std::u16string message = u"Failed to load glyph of char '";
        StatusHandler::log(ERROR, message + ch + u"': " + (const char16_t*)FT_Error_String(error));

        return;
    }

    FT_GlyphSlot slot = face->glyph;

    chars.try_emplace(ch,
                      slot->bitmap.buffer, abs(slot->bitmap.pitch),
                      slot->bitmap.width, slot->bitmap.rows,
                      slot->bitmap_left, slot->bitmap_top,
                      slot->advance.x >> 6);
};

const TextManager::FontHandle::BitmapData* TextManager::FontHandle::get_char(char16_t ch) const
{
    auto it = chars.find(ch);

    if (it == chars.end())
        return nullptr;

    return &it->second;
}

TextManager::FontHandle::BitmapData::BitmapData(unsigned char* src, int _pitch,
                                                unsigned int _width, unsigned int _height,
                                                int _bearing_x, int _bearing_y,
                                                long _advance_x)
    : data(nullptr), width(_width), height(_height), bearing_x(_bearing_x), bearing_y(_bearing_y), advance_x(_advance_x)
{
    // Creating a persisting copy of glyph bitmap raster for caching purposes
    //data = new unsigned char[size];
    //memcpy(data, src, size);
    data = new unsigned char[width * height];

    if (_pitch == width)
    {
        memcpy(data, src, width * height);
    }

    else {
        for (int r = 0; r < height; ++r)
        {
            memcpy(data + r * width,
                   src + r * _pitch,
                   width);
        }
    }
}

TextManager::FontHandle::BitmapData::~BitmapData()
{
    delete[] data;
}
