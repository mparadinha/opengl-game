#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "signals.h"
#include "camera.h"

Camera::Camera(MessageBus* msg_bus, float ratio) : System(msg_bus) {
    projection = glm::perspective(glm::radians(fov), ratio, near, far);

    codes.push_back(MOUSE_MOVE);
    codes.push_back(MOUSE_SCROLL);

    player_pos = glm::vec3(0, 2.f, 0);
    update(0, 0); // initialize vectors
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
    rotation = fmod(rotation, 360);

    up = glm::normalize(glm::vec3(
        -sin(glm::radians(rotation)),
        sqrt(2) * sin(glm::radians(90 - pitch)),
        -cos(glm::radians(rotation))
    ));

    // change the position
    auto diff = glm::vec3(0); // diff from focus point of camera (player) to camera
    diff.y = distance * sin(glm::radians(pitch));
    auto hyp = distance * cos(glm::radians(pitch));
    diff.z = hyp * cos(glm::radians(rotation));
    diff.x = hyp * sin(glm::radians(rotation));
    position = player_pos + diff;

    // update view mat using glm::lookAt
    view = glm::lookAt(position, player_pos, up);
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
