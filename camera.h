#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "message_bus.h"
#include "shader.h"

class Camera : public System {
public:
    Camera(MessageBus* msg_bus, float ratio);

    void handle_message(message_t msg);

    void set_uniforms(Shader& shader);
    glm::mat4 get_view() { return view; }
    void set_projection_uniform(Shader& shader) {
        shader.set_uniform("projection", glm::value_ptr(projection));
    }

    ~Camera();

    glm::vec3 player_pos;

private:
    void update(float dx, float dy);

    std::vector<int> codes;
    
    glm::mat4 view, projection;

    glm::vec3 position; // relative to player
    float distance = 50.0f; // to player

    float sensitivity = 0.1f;

    float fov = 60.0f;
    float near = 1.0f, far = 1000.0f;

    float pitch = 20.0f; // angle between floor and camera
    float rotation = 0.0f; // sideways rotation relative to player
};

#endif // include guard
