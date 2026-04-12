#ifndef SHADER_HEADER
#define SHADER_HEADER

#include <fstream>
#include <sstream>
#include <string>

class Shader {
public:
    Shader(const char* vertex_file_path, const char* fragment_file_path);
    ~Shader();

    // No copying or moving of shaders
    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;
    Shader(Shader&& other) = delete;
    Shader& operator=(Shader&& other) = delete;

    void use() const;
    int get_uniform_loc(const char* name) const;
private:
    unsigned int ID;

    static std::string get_source_code(const char* file_path);
    static unsigned int compile_shader(const char* source_code, int shader_type, int& success);

    // static buffer streams to avoid reconstruction every time get_source_code is called
    static std::ifstream filestream;
    static std::stringstream stringstream;

    friend class ResourceManager;
};

#endif
