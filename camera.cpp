#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "signals.h"
#include "camera.h"

Camera::Camera(MessageBus* msg_bus, float ratio) : System(msg_bus) {
    projection = glm::perspective(glm::radians(fov), ratio, near, far);

    codes.push_back(MOUSE_MOVE);
    codes.push_back(MOUSE_SCROLL);

    player_pos = glm::vec3(0);
    update(0, 0); // initialize vectors
    std::cerr << "position: " << glm::to_string(position) << std::endl;
    std::cerr << "up: " << glm::to_string(up) << std::endl;
    update(0, 0); // to correctly create the up vector
}

Camera::~Camera() {
}

void Camera::handle_message(message_t msg) {
    switch(msg.code) {
        case MOUSE_MOVE:
            update(msg.data[0], msg.data[1]);
            break;
    }
}

void Camera::update(float dx, float dy) {
    // update pitch and rotation
    pitch += sensitivity * dy;
    rotation += sensitivity * dx;
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < 0.0f) pitch = 0.0f;

    up = glm::normalize(glm::vec3(
        -sin(glm::radians(rotation)),
        sin(glm::radians(90 - pitch)),
        -cos(glm::radians(rotation))
    ));
    if(pitch < -0.0f) pitch = 0.0f;

    // change the position
    position = distance * glm::normalize(glm::vec3(
        sin(glm::radians(rotation)),
        sin(glm::radians(pitch)),
        cos(glm::radians(rotation))
    ));
    position += player_pos + glm::vec3(0, 3.6f, 0);

    // update view mat using glm::lookAt
    view = glm::lookAt(position, player_pos, up);
    position += player_pos;
}

void Camera::set_uniforms(Shader& shader) {
    shader.set_uniform("view", glm::value_ptr(view));
    shader.set_uniform("projection", glm::value_ptr(projection));
} 

void Camera::set_projection_uniform(Shader& shader) {
    shader.set_uniform("projection", glm::value_ptr(projection));
}

glm::mat4 Camera::get_view() {
    return view;
}
