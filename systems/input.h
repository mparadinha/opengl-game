#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../message_bus.h"

class Input : public System {
public:
    Input(GLFWwindow* window, MessageBus* msg_bus);
    ~Input();

    void poll_events();
    void poll_mouse();
    int should_close();

    void key_callback(int key, int scancode, int action, int mods);

    void handle_message(message_t msg);

private:
    bool pressed(int code);

    GLFWwindow* window;
};

#endif // INPUT_H include guard
