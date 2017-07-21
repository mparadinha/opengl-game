#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "display.h"

Display::Display(int w, int h, std::string title) {
    // init the glfw library and check for errors
    if(!glfwInit())
        std::cout << "Error start GLFW lib." << std::endl;

    // create a window and openGL context
    window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
    if(window == NULL)
        std::cerr << "Error creating the window/context." << std::endl;

    // we need to set the context being used to the one we just created
    glfwMakeContextCurrent(window);

    // capture mouse movement and hide the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // we already create the GL context here so i'll also init GLEW
    if(glewInit() != GLEW_OK)
        std::cerr << "Error initializing glew." << std::endl;

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
