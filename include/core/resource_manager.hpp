#pragma once

#include "core/handles/entity.hpp"
#include "core/handles/shader.hpp"
#include "core/text_manager.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <map>
#include <string>
#include <variant>
#include <vector>

class ResourceManager {
private:
    struct Drawables {
        std::vector<glm::mat4>                  models;
        std::vector<std::variant<Entity, Text>> entries;
    };
public:
    static void          set_shader(const std::string& key, const char* p1, const char* p2);
    static const Shader& get_shader(const std::string& key);

    static void initialize();
    static void clear();
    static void update_models();

    static glm::mat4 projection;
    static glm::mat4 view;
    static Drawables objects;
private:
    ResourceManager() = delete;
    ResourceManager(const ResourceManager& other) = delete;
    ResourceManager(ResourceManager&& other) = delete;
    ResourceManager& operator=(const ResourceManager& other) = delete;
    ResourceManager& operator=(ResourceManager&& other) = delete;

    static std::map<std::string, Shader> shaders;
    static unsigned int                  UBO;
    static unsigned int                  universal_VAO;
};
