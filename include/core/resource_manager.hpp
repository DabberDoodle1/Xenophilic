#ifndef RESOURCE_MANAGER_HEADER
#define RESOURCE_MANAGER_HEADER

#include "core/handles/entity.hpp"
#include "core/handles/shader.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <map>
#include <string>
#include <vector>

class ResourceManager {
private:
    struct EntityModelWrapper {
        std::vector<glm::mat4> models;
        std::vector<Entity> entities;
    };
public:
    static void set_shader(const std::string& key, const char* p1, const char* p2);
    static const Shader& get_shader(const std::string& key);

    static void initialize();
    static void clear();

    static void update_models();
    static inline void set_modified()
    {
        is_modified = true;
    }

    static glm::mat4 projection;
    static glm::mat4 view;
    static EntityModelWrapper wrapper;
private:
    ResourceManager() = delete;
    ResourceManager(const ResourceManager& other) = delete;
    ResourceManager(ResourceManager&& other) = delete;
    ResourceManager& operator=(const ResourceManager& other) = delete;
    ResourceManager& operator=(ResourceManager&& other) = delete;

    static std::map<std::string, Shader> shaders;
    static bool is_modified;
    static unsigned int UBO;
};

#endif
