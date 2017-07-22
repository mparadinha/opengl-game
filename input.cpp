#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "input.h"
#include "shader.h"
#include "message_bus.h"
#include "signals.h"

void Input::init(GLFWwindow* window, Camera* camera, MessageBus* msg_bus) {
    this->window = window; // having the window as a member is usefull to other functions
    this->camera = camera; // while messaging system doesn't have data passing working we still need this for camera rotation
    this->msg_bus = msg_bus;

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetCursorPosCallback(window, glfw_mouse_callback);

    glfwSetCursorPos(window, 0, 0); // to keep cursor from jumping around in the start
}

Input::~Input() {
}

void Input::handle_message(message_t msg) {
    switch(msg.code) {
        case NEW_FRAME:
            poll_events(); break;
    }
} 

void Input::key_callback(int keycode, int code, int action, int mods) {
    int signal;

    if(action == GLFW_PRESS) {
        switch(keycode) {
            case GLFW_KEY_W:
                signal = MOVE_FRONT;
                break;
            case GLFW_KEY_S:
                signal = MOVE_BACK; 
                break;
            case GLFW_KEY_A:
                signal = MOVE_LEFT;
                break;
            case GLFW_KEY_D:
                signal = MOVE_RIGHT;
                break;
            default:
                return; // best to leave here, or it might send a weird signal
        }
    }
    else if(action == GLFW_RELEASE) {
        switch(keycode) {
            case GLFW_KEY_W:
                signal = STOP_MOVE_FRONT;
                break;
            case GLFW_KEY_S:
                signal = STOP_MOVE_BACK; 
                break;
            case GLFW_KEY_A:
                signal = STOP_MOVE_LEFT;
                break;
            case GLFW_KEY_D:
                signal = STOP_MOVE_RIGHT;
                break;

            case GLFW_KEY_F:
                signal = SWITCH_CAMERA_STYLE;
                break;

            default:
                return; // best to leave here, or it might send a weird signal
        }
    }
    message_t msg = {signal, {}};
    msg_bus->add_message(msg, PRIORITY_NORMAL);
}

void Input::mouse_callback(double x, double y) {
    static float old_x = 0, old_y = 0;
    float xoff, yoff;
    xoff = x - old_x;
    yoff = old_y - y; // reserved because y starts @ the top

    old_x = x; old_y = y; // for next callback

    // broadcast message
    message_t msg = {MOUSE_MOVE, {xoff, yoff}};
    msg_bus->add_message(msg, PRIORITY_NORMAL);
}

void Input::scroll_callback(double x, double y) {
    message_t msg = {MOUSE_SCROLL, {(float) x, (float) y}};
    msg_bus->add_message(msg, PRIORITY_NORMAL);
}

void Input::poll_events() {
    glfwPollEvents();
}

int Input::should_close() {
    return glfwWindowShouldClose(window);
}
