#include "core/engine.hpp"
#include "core/handles/texture.hpp"
#include "core/resource_manager.hpp"
#include "core/status_handler.hpp"
#include "core/text_manager.hpp"
#include "special/video_encoder.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

inline double clamp(double min, double value, double max)
{
    if (value >= max)
        return max;

    if (value <= min)
        return min;

    return value;
}

Engine::Engine(unsigned int width, unsigned int height, const char* title): width(width), height(height)
{
    // Initializing GLFW and GLAD
    if (!glfwInit())
    {
        StatusHandler::log(ERROR, "GLFW LIBRARY NOT LOADED.");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (window == nullptr)
    {
        StatusHandler::log(ERROR, "WINDOW NOT CREATED.");
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        StatusHandler::log(ERROR, "GLAD LIBRARY NOT LOADED.");
        return;
    }

    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Engine::~Engine()
{
    ResourceManager::clear();

    if (window != nullptr)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

void Engine::start()
{
    // Eager-initializing resources
    ResourceManager::initialize();
    ResourceManager::projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
    ResourceManager::view = glm::mat4(1.0f);

    TextManager::initialize();
    TextManager::load_font("default", "res/fonts/SairaStencil-SemiBold.ttf");
    TextManager::set_font("default");

    ResourceManager::wrapper.entities[0].pos_x = -75.0f;
    ResourceManager::wrapper.models[0] = glm::scale(glm::translate(glm::mat4(1.0f),
                                                                   glm::vec3(-150.0f, 75.0f, 0.0f)),
                                                    glm::vec3(150.0f, 150.0f, 1.0f));

    ResourceManager::set_shader("default", "res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    ResourceManager::set_shader("texture", "res/shaders/texture_vertex.glsl", "res/shaders/texture_fragment.glsl");
    const Shader& def_shader = ResourceManager::get_shader("default");
    const Shader& tex_shader = ResourceManager::get_shader("texture");

    unsigned int text_w1, text_h1;
    unsigned int text_w2, text_h2;
    unsigned int text_w3, text_h3;
    Texture line1(TextManager::create_text(u"GUYS, I ACTUALLY DID IT!", text_w1, text_h1));
    Texture line2(TextManager::create_text(u"I MANAGED TO DO TEXT RENDERING.", text_w2, text_h2));
    Texture line3(TextManager::create_text(u"I CAN FINALLY UPDATE MY GITHUB.", text_w3, text_h3));

    def_shader.use();
    glUniform1i(def_shader.get_uniform_loc("index"), 0);
    tex_shader.use();
    glUniform1i(tex_shader.get_uniform_loc("tex"), 0);

    float aspect_ratio1 = static_cast<float>(text_w1) / static_cast<float>(text_h1);
    float aspect_ratio2 = static_cast<float>(text_w2) / static_cast<float>(text_h2);
    float aspect_ratio3 = static_cast<float>(text_w3) / static_cast<float>(text_h3);
    ResourceManager::wrapper.models[1] = glm::scale(glm::translate(glm::mat4(1.0f),
                                                                   glm::vec3(static_cast<float>(width) * 0.5f,
                                                                             static_cast<float>(height) * 0.575f,
                                                                             0.0f)),
                                                    glm::vec3(static_cast<float>(height) * 0.0625f * aspect_ratio1,
                                                              static_cast<float>(height) * 0.0625f,
                                                              1.0f));
    ResourceManager::wrapper.models[2] = glm::scale(glm::translate(glm::mat4(1.0f),
                                                                   glm::vec3(static_cast<float>(width) * 0.5f,
                                                                             static_cast<float>(height) * 0.5f,
                                                                             0.0f)),
                                                    glm::vec3(static_cast<float>(height) * 0.0625f * aspect_ratio2,
                                                              static_cast<float>(height) * 0.0625f,
                                                              1.0f));
    ResourceManager::wrapper.models[3] = glm::scale(glm::translate(glm::mat4(1.0f),
                                                                   glm::vec3(static_cast<float>(width) * 0.5f,
                                                                             static_cast<float>(height) * 0.425f,
                                                                             0.0f)),
                                                    glm::vec3(static_cast<float>(height) * 0.0625f * aspect_ratio3,
                                                              static_cast<float>(height) * 0.0625f,
                                                              1.0f));

    double delta_time;
    double previous = glfwGetTime(), current;

    #ifdef VIDEO_RECORDING
    std::vector<unsigned char> frame_buffer(width * height * 3);
    std::vector<unsigned char> frame_flipped_buffer(width * height * 3);
    alignas(64) VideoEncoder encoder("output.mp4", width, height, 60);
    const unsigned int rgb_width = width * 3;
    unsigned int index = 1;
    #endif

    glActiveTexture(GL_TEXTURE0);
    while (!glfwWindowShouldClose(window))
    {
        current = glfwGetTime();
        delta_time = current - previous;
        previous = current;

        handle_input(delta_time);
        handle_physics();
        ResourceManager::update_models();

        glClear(GL_COLOR_BUFFER_BIT);

        // Player slot
        def_shader.use();

        ResourceManager::wrapper.entities[0].bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Text slots
        tex_shader.use();

        glUniform1i(tex_shader.get_uniform_loc("index"), 1);
        ResourceManager::wrapper.entities[1].bind();
        line1.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniform1i(tex_shader.get_uniform_loc("index"), 2);
        ResourceManager::wrapper.entities[2].bind();
        line2.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniform1i(tex_shader.get_uniform_loc("index"), 3);
        ResourceManager::wrapper.entities[3].bind();
        line3.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        #ifdef VIDEO_RECORDING
        // Save frames here
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, frame_buffer.data());

        for (int i = 0; i < height; ++i)
        {
             memcpy(frame_flipped_buffer.data() + i * rgb_width,
                    frame_buffer.data() + frame_buffer.capacity() - i * rgb_width,
                    rgb_width);
        }

        encoder.add_frame(frame_flipped_buffer.data(), index++);
        #endif

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::handle_input(double delta_time)
{
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        ResourceManager::wrapper.entities[0].vel_x += 100.0f * delta_time;
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        ResourceManager::wrapper.entities[0].vel_x -= 100.0f * delta_time;
    else
        ResourceManager::wrapper.entities[0].vel_x = 0.0f;

    ResourceManager::wrapper.entities[0].vel_x = clamp(-25.0, ResourceManager::wrapper.entities[0].vel_x, 25.0);
}

void Engine::handle_physics()
{
    ResourceManager::wrapper.entities[0].pos_x += ResourceManager::wrapper.entities[0].vel_x;
    ResourceManager::wrapper.entities[0].pos_y += ResourceManager::wrapper.entities[0].vel_y;

    ResourceManager::wrapper.models[0] = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(ResourceManager::wrapper.entities[0].pos_x, 75.0f, 0.0f)), glm::vec3(150.0f, 150.0f, 1.0f));
    ResourceManager::set_modified();
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE  || key == GLFW_KEY_Q && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
