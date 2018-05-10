#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>

#include <GLFW/glfw3.h> // for the GLFWwindow member

//TODO: why is this a class????? no need I think. refactor
class Display {
public:
    Display(int width, int height, std::string title); // constructor

    void set_clear_color(float r, float g, float b, float a);
    void clear();
    void swap_buffers();

    ~Display(); // destructor

    GLFWwindow* window;
};

#endif // DISPLAY_H
