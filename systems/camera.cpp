#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "camera.h"

#include "../signals.h"
#include "../components/pos_rot_scale.h"
#include "../components/rigid_body.h"
#include "../components/camera.h"
#include "../components/bounding_volumes.h"

#include "../entity_pool_global.h"

CameraUpdater::CameraUpdater(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing camera updater...\n";

    // create camera and add to the special pool
    //glm::vec3 pos(10, 20.5, 10), scale(1, 2.5, 1);
    glm::vec3 pos(1.5, 2, 1.5), scale(1, 2.5, 1);
    pos_rot_scale_t* c_pos = new pos_rot_scale_t({pos, scale, 45, 15, 0});
    rigid_body_t* rb_c = new rigid_body_t({pos, {0, 0, 0}, scale, 0, 0, 0, 1 / 80.0, false, {0.25, 0.4}});
    aabb_t* c_bv = new aabb_t({pos, scale});
    camera_t* c = new camera_t({glm::mat4(1), glm::perspective(glm::radians(60.0), 1.67, 0.1, 1000.0), 60});
    camera = new Entity;
    camera->components[POS_ROT_SCALE] = c_pos;
    camera->components[CAMERA] = c;
    camera->components[RIGID_BODY] = rb_c;
    camera->components[AABB] = c_bv;
    camera->bitset = POS_ROT_SCALE | CAMERA | RIGID_BODY | AABB;

    e_pool.add_special(camera, CAMERA);
}

CameraUpdater::~CameraUpdater() {
}

void CameraUpdater::handle_message(message_t msg) {
    switch(msg.code) {
    case JUMP:
        moving = true;
        jump();
        break;

    case PLAYER_MOVE:
        // need to set moving state for the update function
        if(msg.data[0] == 0 && msg.data[1] == 0) {
            moving = false;
        }
        else {
            moving = true;
        }

        update_1st_person(msg.data[0], msg.data[1], 0, 0);
        break;

    case MOUSE_MOVE:
        update_look(msg.data[0], msg.data[1]);
        update_1st_person(0, 0, msg.data[0], msg.data[1]);
        break;
    }
}

void CameraUpdater::jump() {
    rigid_body_t* rb = (rigid_body_t*) camera->components[RIGID_BODY];
    if(abs(rb->vel[1]) < 0.05) {
        rb->vel += glm::vec3(0, 10, 0);
    }
}

void CameraUpdater::update_move(float forward, float right) {
    if(camera_style == "fps") {
    }
    else if(camera_style == "3rd") {
    }
}

void CameraUpdater::update_look(float dx, float dy) {
}

void CameraUpdater::update_1st_person(float forward, float right, float dx, float dy) {
    const static float vel = 10;
    const static float sens = 0.02;

    bool update_pos = (forward != 0 || right != 0), update_angles = (dx != 0 || dy != 0);
    //TODO: calculate the view matrix every frame on the renderer system.
    // this way here on the camera system we don't need to worry about having
    // to update the camera even when nothing (aka the user didn't do anything)
    // happens. this probably requires us to save the camera's local axis on the component

    //TODO: use dx and dy as percentage of total screen so that sensitivity makes sense
    // and is independent of the screen size and works the same for x and y directions

    // update the orientation and position of the camera
    pos_rot_scale_t* prs = (pos_rot_scale_t *) camera->components[POS_ROT_SCALE];
    if(update_angles) {
        prs->pitch += sens * dy;
        prs->yaw -= sens * dx;
    }
    // correct pitch
    if(prs->pitch > 89.9) prs->pitch = 89.9;
    if(prs->pitch < -89.9) prs->pitch = -89.9;

    // calculate camera axis after rotations
    float cos_pitch = cos(glm::radians(prs->pitch)); float cos_yaw = cos(glm::radians(prs->yaw));
    float sin_pitch = sin(glm::radians(prs->pitch)); float sin_yaw = sin(glm::radians(prs->yaw));
    glm::vec3 xaxis(cos_yaw, 0, -sin_yaw),
        yaxis(sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch),
        zaxis(sin_yaw * cos_pitch, -sin_pitch, cos_pitch * cos_yaw);

    // move camera
    rigid_body_t* rb = (rigid_body_t*) camera->components[RIGID_BODY];
    if(!moving) {
        //rb->vel = glm::vec3(0);
        rb->vel[0] = 0; rb->vel[2] = 0;
    }
    else if(update_pos) {
        glm::vec3 dir = glm::normalize(right * xaxis - forward * zaxis);

        // control while in the air needs to be restricted
        float velocity = abs(rb->vel[1]) > 0.05 ? 0.3 * vel : vel;

        // lock movement to xz plane because we're using an fps style camera
        rb->vel[0] = velocity * dir[0];
        rb->vel[2] = velocity * dir[2];
    }

    // update the view matrix for the camera
    ((camera_t* ) camera->components[CAMERA])->view = glm::mat4({
        xaxis.x, yaxis.x, zaxis.x, 0,
        xaxis.y, yaxis.y, zaxis.y, 0,
        xaxis.z, yaxis.z, zaxis.z, 0,
        -dot(xaxis, prs->pos), -dot(yaxis, prs->pos), -dot(zaxis, prs->pos), 1});
}
