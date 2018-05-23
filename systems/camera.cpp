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
    pos_rot_scale_t* c_pos = new pos_rot_scale_t({glm::vec3(10, 10, 10), glm::vec3(1), 0, 0, 0});
    rigid_body_t* rb_c = new rigid_body_t({{10, 10, 10}, {1, 1, 1}, {}, 0, 0, 0, true});
    aabb_t* c_bv = new aabb_t({{10, 10, 10}, {1, 1, 1}});
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
    if(msg.code != NEW_FRAME && msg.code != MOUSE_MOVE) {
        std::cout << "camera::handle_message:msg.code: " << msg.code << std::endl;
    }
    switch(msg.code) {
    //HACK: this (in a perfect world) shouldn't need to be here
    // we need to update the view matrix in case the camera is moving
    // even if there no alterations in the movement keys/mouse
    case NEW_FRAME:
        //update_1st_person(0, 0, 0, 0);
        break;

    case PLAYER_MOVE:
        // need to set moving state for the update function
        if(msg.data[0] == 0 && msg.data[1] == 0) {
            moving = false;
        }
        else {
            moving = true;
        }
        std::cout << "PLAYER_MOVE recieved. moving = " << moving << std::endl;

        update_1st_person(msg.data[0], msg.data[1], 0, 0);
        break;

    case MOUSE_MOVE:
        update_look(msg.data[0], msg.data[1]);
        update_1st_person(0, 0, msg.data[0], msg.data[1]);
        break;
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
    // this line is commented because there can be nothing changed but the camera is still
    // moving and need to update the view matrix.
    //TODO: maybe we should update the view matrix in the renderer before before rendering
    // the other things
    //if(!update_pos && !update_angles) return; // nothing to do, don't need to waste time
    //if(update_pos) std::cout << "update_1st_person(forward=" << forward << ", right=" << right << ", dx=" << dx << ", dy=" << dy << ")\n";

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
    if(moving) {
        //std::cout << "updating pos: " << forward << ", " << right << std::endl;
        glm::vec3 dir = glm::normalize(right * xaxis - forward * zaxis);
        dir.y = 0;
        rigid_body_t* rb = (rigid_body_t*) camera->components[RIGID_BODY];
        rb->vel = vel * dir;
        std::cout << "on camera update. vel is now: " << glm::to_string(rb->vel) << std::endl;
        //prs->pos += vel * dir;
        //prs->pos = player_prs->pos + glm::vec3(0, 2, 0);
    }
    else {
        //std::cout << "stopped moving!\n";
        rigid_body_t* rb = (rigid_body_t*) camera->components[RIGID_BODY];
        rb->vel = glm::vec3(0);
    }

    // update the view matrix for the camera
    if(update_pos || update_angles || moving) {
        ((camera_t* ) camera->components[CAMERA])->view = glm::mat4({
            xaxis.x, yaxis.x, zaxis.x, 0,
            xaxis.y, yaxis.y, zaxis.y, 0,
            xaxis.z, yaxis.z, zaxis.z, 0,
            -dot(xaxis, prs->pos), -dot(yaxis, prs->pos), -dot(zaxis, prs->pos), 1});
    }
}
