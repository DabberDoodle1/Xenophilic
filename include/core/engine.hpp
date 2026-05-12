#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Engine {
public:
    Engine(unsigned int width, unsigned int height, const char* title);
    ~Engine();

    void start();
private:
    void handle_input();
    void handle_physics(double delta_time);

    GLFWwindow* window;
    const unsigned int width;
    const unsigned int height;

    double cursor_x;
    double cursor_y;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};
