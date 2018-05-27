#include <iostream>

#include <glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "input.h"
#include "../message_bus.h"
#include "../signals.h"

namespace glfw_callbacks {
    static Input* input_system;
    void keys(GLFWwindow* window, int key, int scancode, int action, int mods);
}

void glfw_callbacks::keys(GLFWwindow* window, int key, int scancode, int action, int mods) {
    glfw_callbacks::input_system->key_callback(key, scancode, action, mods);
}

Input::Input(GLFWwindow* window, MessageBus* msg_bus) : System(msg_bus), window(window) {
    glfw_callbacks::input_system = this;
    glfwSetKeyCallback(window, glfw_callbacks::keys);
}

Input::~Input() {}

void Input::handle_message(message_t msg) {
    switch(msg.code) {
        case NEW_FRAME:
            poll_events();
            poll_mouse();
            break;
    }
}

void Input::key_callback(int key, int scancode, int action, int mods) {
    //if(action != GLFW_REPEAT) std::cout << "key_callback(key=" << key << ", scancode=" << scancode << ", action=" << action << ", mods=" << mods << ")" << std::endl;
    // movement changes
    if(key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {
        message_t msg = {PLAYER_MOVE,
            {(float) pressed(GLFW_KEY_W) - (float) pressed(GLFW_KEY_S), // forward/backward movement
             (float) pressed(GLFW_KEY_D) - (float) pressed(GLFW_KEY_A)} // right/left movement
        };

        send_msg(msg);
    }

    // toggle bounding boxes
    if(key == GLFW_KEY_B && action != GLFW_REPEAT) {
        send_msg({TOGGLE_BB});
    }

    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        send_msg({JUMP});
    }
}

void Input::poll_mouse() {
    static float lastx = 0, lasty = 0;

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // calc diffs
    float diffx = x - lastx;
    float diffy = lasty - y; // y starts at top

    // post to the message bus
    message_t msg = {MOUSE_MOVE, {diffx, diffy}};
    send_msg(msg);

    // save x and y for next call
    lastx = x; lasty = y;
}

void Input::poll_events() {
    glfwPollEvents();
}

int Input::should_close() {
    return glfwWindowShouldClose(window);
}

bool Input::pressed(int code) {
    return glfwGetKey(window, code) == GLFW_PRESS;
}
