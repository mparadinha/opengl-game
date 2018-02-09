#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "message_bus.h"

//NOTE: not singleton right now
// Singleton class to allow setting glfw callbacks to the member functions
// job of this class is to translate key presses into actions and report these
// to the (yet to be implemented) messaging system
class Input : public System {
public:
    Input(GLFWwindow* window, MessageBus* msg_bus);
/*
    static Input& get_first_instance() {
    static Input& get_instance() { // singleton accessed via get_instance
        static Input instance; // creates the single instance
        return instance;
    }
    void init(GLFWwindow* window, MessageBus* msg_bus); // actually do initialization

    // pass-through function to the actual implementation
    static void glfw_key_callback(GLFWwindow* w, int k, int c, int a, int m) {
        Input::get_instance().key_callback(k, c, a, m);
    }
    static void glfw_mouse_callback(GLFWwindow* window, double x, double y) {
        Input::get_instance().mouse_callback(x, y);
    }
    static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        Input::get_instance().scroll_callback(xoffset, yoffset);
    }
    // actual callback implementations are member functions (no need for globals)
    void key_callback(int key, int code, int action, int mods);
    void mouse_callback(double x, double y);
    void scroll_callback(double x, double y);
*/

    void poll_events();
    void poll_keys();
    void poll_mouse();
    int should_close();

    void handle_message(message_t msg);

    ~Input();

private:
/*
    Input(void) {} // private constructor to allow only 1 instance
    Input(Input const&); // prevents copies
    void operator=(Input const&); // prevent assignements
*/

    float pressed(int code);

    GLFWwindow* window;
};

#endif // INPUT_H include guard
