#include <iostream>

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.h"
#include "fps_camera.h"
#include "message_bus.h"
#include "signals.h"

FPSCamera::FPSCamera(glm::vec3 position, float ratio) {
    pos = position;
    projection = glm::perspective(glm::radians(fov), ratio, near, far);

    update_vectors(); update_view();
}

FPSCamera::~FPSCamera() {
}

void FPSCamera::set_all_uniforms(Shader& shader) {
    shader.set_uniform("projection", &projection[0][0]);
    shader.set_uniform("view", &view[0][0]);
    shader.set_uniform("view_pos", pos);
}

void FPSCamera::handle_message(message_t msg) {
    switch(msg.code) {
        case MOVE_FRONT:
            direction.x = 1; break;
        case STOP_MOVE_FRONT:
            direction.x = 0; break;
        case MOVE_BACK:
            direction.y = 1; break;
        case STOP_MOVE_BACK:
            direction.y = 0; break;
        case MOVE_RIGHT:
            direction.z = 1; break;
        case STOP_MOVE_RIGHT:
            direction.z = 0; break;
        case MOVE_LEFT:
            direction.w = 1; break;
        case STOP_MOVE_LEFT:
            direction.w = 0; break;

        case MOUSE_MOVE:
            mouse_move(msg.data[0], msg.data[1]); break;

        case MOUSE_SCROLL:
            // nothing yet
            break;

        case SWITCH_CAMERA_STYLE:
            free = !free; std::cerr << "switching camera style...\n"; break;

        case NEW_FRAME:
            update(msg.data[0]); break;
    }
}

void FPSCamera::mouse_move(float xdiff, float ydiff) {
    xdiff *= sensitivity;    
    ydiff *= sensitivity;    

    yaw += xdiff;
    pitch += ydiff;
    // weird stuff might happend when pitch goes all the way around, so we keep it contained
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    update_vectors();
    update_view();
}

void FPSCamera::update_vectors() {
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}

void FPSCamera::update_view() {
    view = glm::lookAt(pos, pos + front, up);
}

void FPSCamera::update(float dt) {
    // direction of where camera will travel
    glm::vec3 dir = front * direction.x - front * direction.y + right * direction.z - right * direction.w;

    // 0 direction means we do nothing. plus it gives rise to nan when we try to compute the normal vector
    if(dir.x == 0 && dir.y == 0 && dir.z == 0) return;

    // the actual difference in position
    glm::vec3 diff = (speed * dt) * glm::normalize(dir);

    // make sure camera does not leave the xz plane if using a fps style camera system
    if(free) diff.y = 0;

    pos += diff;

    update_vectors();
    update_view();
}
