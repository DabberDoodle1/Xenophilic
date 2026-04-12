#ifndef TEXT_MANAGER_HEADER
#define TEXT_MANAGER_HEADER

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>

class TextManager {
public:
    static void initialize();
    static void clear();

    static void load_font(const char* key, const char* file_path);
    static void set_font(const char* key);
    static unsigned int create_text(const char16_t* text, unsigned int& _width, unsigned int& _height);

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
            BitmapData(unsigned char* _data, int _pitch,
                       unsigned int _width, unsigned int _height,
                       int _bearing_x, int _bearing_y,
                       long _advance_x);
            ~BitmapData();

            unsigned char* data;
            unsigned int   width;
            unsigned int   height;
            int            bearing_x;
            int            bearing_y;
            long           advance_x;
        };

        void set_char(char16_t ch);
        const BitmapData* get_char(char16_t ch) const;

    private:
        std::map<char16_t, BitmapData> chars;
        FT_Face                        face;
    };

    static std::map<std::string, FontHandle> fonts;
    static FontHandle*                       active_font;
    static FT_Library                        FT;
};

#endif
