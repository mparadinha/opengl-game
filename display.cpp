#include <string>
#include <iostream>

#include <glad.h>

#include <GLFW/glfw3.h>

#include "display.h"

void error_call(int code, char* error_msg) {
    std::cerr << "[GLFWerror] - " << error_msg << std::endl;
}

Display::Display(int w, int h, std::string title) {
    glfwSetErrorCallback((GLFWerrorfun) error_call);

    // init the glfw library and check for errors
    if(!glfwInit())
        std::cerr << "Error start GLFW lib." << std::endl;

    // opengl version hints for creating the context below
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create a window and openGL context
    window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
    if(window == NULL)
        std::cerr << "Error creating the window/context." << std::endl;

    // we need to set the context being used to the one we just created
    glfwMakeContextCurrent(window);

    glfwSwapInterval(0); // prevent glfw from waiting between displaying frames

    // laod all the GL functions with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        std::cerr << "Failed to initialize GLAD" << std::endl;

    // capture mouse movement and hide the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

Display::~Display() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Display::set_clear_color(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void Display::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::swap_buffers() {
    glfwSwapBuffers(window);
}
