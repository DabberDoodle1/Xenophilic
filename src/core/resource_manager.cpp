#include "core/resource_manager.hpp"
#include "core/status_handler.hpp"
#include <cstring>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>

alignas(64) glm::mat4                     ResourceManager::projection(1.0f);
alignas(64) glm::mat4                     ResourceManager::view(1.0f);
alignas(64) ResourceManager::Drawables    ResourceManager::objects;
alignas(64) std::map<std::string, Shader> ResourceManager::shaders;

unsigned int ResourceManager::UBO;
unsigned int ResourceManager::universal_VAO;

constexpr unsigned int models_size = 256 * sizeof(glm::mat4);
void* ubo_pointer = nullptr;

void ResourceManager::set_shader(const std::string& key, const char* p1, const char* p2)
{
    shaders.try_emplace(key, p1, p2);
}

const Shader& ResourceManager::get_shader(const std::string& key)
{
    auto it = shaders.find(key);

    if (it == shaders.end())
    {
        // Fallback to default shader (embedded) if desired shader isn't found
        it = shaders.find("default");

        std::string base("Failed to find shader under key \"");
        StatusHandler::log(WARNING, base + key + "\" therefore returning default shader");

        return it->second;
    }

    return it->second;
}

void ResourceManager::initialize()
{
    objects.models.reserve(256);
    objects.entries.reserve(256);

    glCreateBuffers(1, &UBO);
    glNamedBufferStorage(UBO, models_size, nullptr, GL_MAP_WRITE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);

    float vertices[] = {
        0.5f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f,
        -0.5, 0.5f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    unsigned int BO[2];

    glGenVertexArrays(1, &universal_VAO);
    glGenBuffers(2, BO);

    glBindVertexArray(universal_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, BO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO[1]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDeleteBuffers(2, BO);
    glBindVertexArray(universal_VAO);
}

void ResourceManager::clear()
{
    objects.entries.clear();
    objects.models.clear();
    shaders.clear();
    glDeleteBuffers(1, &UBO);
}

void ResourceManager::update_models()
{
    glm::mat4 MVPs[256]{};

    for (int i = 0; i < objects.models.size(); ++i)
    {
        MVPs[i] = projection * view * objects.models[i];
    }

    ubo_pointer = glMapNamedBufferRange(UBO, 0, models_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    memcpy(ubo_pointer, MVPs, models_size);
    glUnmapNamedBuffer(UBO);
}
