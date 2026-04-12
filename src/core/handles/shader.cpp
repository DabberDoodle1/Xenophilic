#include "core/handles/shader.hpp"
#include "core/status_handler.hpp"
#include <glad/glad.h>

std::ifstream Shader::filestream;
std::stringstream Shader::stringstream;

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path): ID(0)
{
    std::string vertex_source = get_source_code(vertex_file_path);
    std::string fragment_source = get_source_code(fragment_file_path);
    const char* vertex_source_value = vertex_source.c_str();
    const char* fragment_source_value = fragment_source.c_str();

    int success;
    char log[512];

    unsigned int vertex_shader;
    unsigned int fragment_shader;

    vertex_shader = compile_shader(vertex_source_value, GL_VERTEX_SHADER, success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, log);
        glDeleteShader(vertex_shader);

        std::string base("Failed to compile vertex shader: ");

        StatusHandler::log(ERROR, base + log);
        return;
    }

    fragment_shader = compile_shader(fragment_source_value, GL_FRAGMENT_SHADER, success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, log);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        std::string base("Failed to compile fragment shader because ");

        StatusHandler::log(ERROR, base + log);
        return;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    glLinkProgram(ID);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::~Shader()
{
    if (ID != 0)
    {
        glDeleteProgram(ID);
    }
}

void Shader::use() const
{
    glUseProgram(ID);
}

int Shader::get_uniform_loc(const char* name) const
{
    return glGetUniformLocation(ID, name);
}


std::string Shader::get_source_code(const char* file_path)
{
    std::string source_code;

    filestream.open(file_path);
    if (!filestream.is_open())
    {
        std::string base("Failed to open file at: ");

        StatusHandler::log(ERROR, base + file_path);
    }
    stringstream << filestream.rdbuf();
    source_code = stringstream.str();

    stringstream.str("");
    stringstream.clear();
    filestream.close();

    return source_code;
}

unsigned int Shader::compile_shader(const char* source_code, int shader_type, int& success)
{
    unsigned int shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source_code, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    return shader;
}
