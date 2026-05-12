#include "core/engine.hpp"
#include "core/resource_manager.hpp"
#include "core/status_handler.hpp"
#include "core/text_manager.hpp"
#include "special/video_encoder.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Text* selected = nullptr;

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        StatusHandler::log(ERROR, "GLAD LIBRARY NOT LOADED.");
        return;
    }

    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Eager-initializing resources
    ResourceManager::initialize();

    ResourceManager::projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    ResourceManager::view       = glm::mat4(1.0f);

    ResourceManager::set_shader("default", "res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    ResourceManager::set_shader("texture", "res/shaders/texture_vertex.glsl", "res/shaders/texture_fragment.glsl");

    TextManager::initialize();
    TextManager::load_font("default", "res/fonts/NotoSansJP-Regular.ttf");
    TextManager::set_font("default");
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
    #ifdef VIDEO_RECORDING
    std::vector<unsigned char> frame_buffer(width * height * 3);
    std::vector<unsigned char> frame_flipped_buffer(width * height * 3);
    alignas(64) VideoEncoder encoder("output.mp4", width, height, 60);
    const unsigned int rgb_width = width * 3;
    unsigned int index = 1;
    #endif

    unsigned int dip;
    ResourceManager::objects.entries.emplace_back(TextManager::create_text(u"アークナイツ | あーくないつ", MEDIUM, dip));
    ResourceManager::objects.models.emplace_back();

    Text& text1  = std::get<Text>(ResourceManager::objects.entries[0]);
    auto& model1 = ResourceManager::objects.models[0];

    text1.x = width / 2.0f;
    text1.y = height / 2.0f - dip;
    model1  = glm::translate(glm::mat4(1.0f), glm::vec3(text1.x + 1, text1.y + 1, 0.0f));
    model1  = glm::scale(model1, glm::vec3(text1.w, text1.h, 1.0f));

    const Shader& def_shader = ResourceManager::get_shader("default");
    const Shader& tex_shader = ResourceManager::get_shader("texture");

    tex_shader.use();
    glActiveTexture(GL_TEXTURE0);

    double delta_time;
    double prev_time;
    double curr_time;

    prev_time = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        curr_time  = glfwGetTime();
        delta_time = curr_time - prev_time;
        prev_time  = curr_time;

        handle_input();
        handle_physics(delta_time);

        glClear(GL_COLOR_BUFFER_BIT);

        text1.ID.bind();
        glUniform1i(tex_shader.get_uniform_loc("index"), 0);
        glUniform1i(tex_shader.get_uniform_loc("is_hovering"), selected == &text1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        #ifdef VIDEO_RECORDING
        // Encode current frame
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

void Engine::handle_input()
{
    glfwGetCursorPos(window, &cursor_x, &cursor_y);

    for (int i = 0; i < ResourceManager::objects.entries.size(); ++i) {
        if (Text* text  = std::get_if<Text>(&ResourceManager::objects.entries[i])) {
            const float half_w1 = text->w / 2.0f;
            const float half_h1 = text->h / 2.0f;

            if (cursor_x >= text->x - half_w1 && cursor_x <= text->x + half_w1 &&
                    cursor_y >= text->y - half_h1 && cursor_y <= text->y + half_h1) {
                selected = text;
            } else {
                selected = nullptr;
            }
        }
    }
}

void Engine::handle_physics(double delta_time)
{
    ResourceManager::update_models();
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    ResourceManager::projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
}

void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE  || key == GLFW_KEY_Q && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Engine::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && selected) {
        StatusHandler::log(SUCCESS, "Clicked!");
    }
}
