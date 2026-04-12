#ifndef ENGINE_HEADER
#define ENGINE_HEADER

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Engine {
public:
    Engine(unsigned int width, unsigned int height, const char* title);
    ~Engine();

    void start();
private:
    void handle_input(double delta_time);
    void handle_physics();

    GLFWwindow* window;
    const unsigned int width;
    const unsigned int height;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif
