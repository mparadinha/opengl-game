#include <cmath>

#include <glm/gtx/string_cast.hpp>

#include "camera.h"

#include "../signals.h"
#include "../components/pos_rot_scale.h"
#include "../components/rigid_body.h"
#include "../components/camera.h"

#include "../entity_pool_global.h"

CameraUpdater::CameraUpdater(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing camera updater...\n";

    // create camera and add to the special pool
    pos_rot_scale_t* c_pos = new pos_rot_scale_t({glm::vec3(10, 10, 10), glm::vec3(1), 0, 0, 0});
    rigid_body_t* rb_c = new rigid_body_t({{10, 10, 10}, {}, {}, 0, 0, 0, true});
    camera_t* c = new camera_t({glm::mat4(1), glm::perspective(glm::radians(60.0), 1.67, 0.1, 1000.0), 60});
    camera = new Entity;
    camera->components[POS_ROT_SCALE] = c_pos;
    camera->components[CAMERA] = c;
    camera->components[RIGID_BODY] = rb_c;
    camera->bitset = POS_ROT_SCALE | CAMERA | RIGID_BODY;

    e_pool.add_special(camera, CAMERA);
}

CameraUpdater::~CameraUpdater() {
}

void CameraUpdater::handle_message(message_t msg) {
    switch(msg.code) {
    case PLAYER_MOVE:
        update_move(msg.data[0], msg.data[1]);
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
    const static float vel = 0.05;
    const static float sens = 0.02;

    bool update_pos = (forward != 0 || right != 0), update_angles = (dx != 0 || dy != 0);

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
    if(update_pos) {
        prs->pos += vel * glm::normalize(right * xaxis - forward * zaxis);
        //prs->pos = player_prs->pos + glm::vec3(0, 2, 0);
    }

    // update the view matrix for the camera
    if(update_pos || update_angles) {
        ((camera_t* ) camera->components[CAMERA])->view = glm::mat4({
            xaxis.x, yaxis.x, zaxis.x, 0,
            xaxis.y, yaxis.y, zaxis.y, 0,
            xaxis.z, yaxis.z, zaxis.z, 0,
            -dot(xaxis, prs->pos), -dot(yaxis, prs->pos), -dot(zaxis, prs->pos), 1});
    }
}
