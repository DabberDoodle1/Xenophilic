#include "core/resource_manager.hpp"
#include "core/status_handler.hpp"
#include <cstring>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>

alignas(64) glm::mat4 ResourceManager::projection(1.0f);
alignas(64) glm::mat4 ResourceManager::view(1.0f);
alignas(64) ResourceManager::EntityModelWrapper ResourceManager::wrapper;
alignas(64) std::map<std::string, Shader> ResourceManager::shaders;

unsigned int ResourceManager::UBO;
bool ResourceManager::is_modified = false;

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
    wrapper.entities.reserve(256);
    wrapper.entities.resize(256, Entity());

    for (int i = 0; i < 256; ++i)
    {
        wrapper.entities[i].initialize();
    }

    wrapper.models.reserve(256);
    wrapper.models.resize(256, glm::mat4());

    glCreateBuffers(1, &UBO);
    glNamedBufferStorage(UBO, models_size, nullptr, GL_MAP_WRITE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
}

void ResourceManager::clear()
{
    wrapper.entities.clear();
    wrapper.models.clear();
    shaders.clear();
    glDeleteBuffers(1, &UBO);
}

void ResourceManager::update_models()
{
    static bool yes = false;
    if (!is_modified)
    {
        return;
    }

    glm::mat4 MVPs[256]{};

    for (int i = 0; i < 256; ++i)
    {
        MVPs[i] = projection * view * wrapper.models[i];
    }

    ubo_pointer = glMapNamedBufferRange(UBO, 0, models_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    memcpy(ubo_pointer, MVPs, models_size);
    glUnmapNamedBuffer(UBO);
    is_modified = false;
}
