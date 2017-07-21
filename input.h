#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "camera.h"
#include "message_bus.h"

typedef struct {
    float x, y;
    float xoffset, yoffset;
    bool seen;
} mouse_info_t;

typedef struct {
    int key, code, action, mods;
    bool dirty; // this state has already been read
} key_info_t;

// Singleton class to allow setting glfw callbacks to the member functions
// job of this class is to translate key presses into actions and report these
// to the (yet to be implemented) messaging system
class Input : public System {
public:
    static Input& get_instance() { // singleton accessed via get_instance
        static Input instance; // creates the single instance
        return instance;
    }
    void init(GLFWwindow* window, Camera* camera, MessageBus* msg_bus); // actually do initialization

    // pass-through function to the actual implementation
    static void glfw_key_callback(GLFWwindow* w, int k, int c, int a, int m) {
        Input::get_instance().key_callback(k, c, a, m);
    }
    static void glfw_mouse_callback(GLFWwindow* window, double x, double y) {
        Input::get_instance().mouse_callback(x, y);
    }
    // actual callback implementations are member functions (no need for globals)
    void key_callback(int key, int code, int action, int mods);
    void mouse_callback(double x, double y);

    void poll_events();
    void poll_mouse();
    int should_close();

    virtual void handle_message(message_t msg);

    ~Input();

    std::string name = "Input";

private:
    Input(void) {} // private constructor to allow only 1 instance
    Input(Input const&); // prevents copies
    void operator=(Input const&); // prevent assignements

    mouse_info_t mouse;
    key_info_t key;

    GLFWwindow* window;
    Camera* camera;
    MessageBus* msg_bus;
};

#endif // INPUT_H include guard
