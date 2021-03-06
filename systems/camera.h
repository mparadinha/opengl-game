#ifndef SYSTEM_CAMERA_H
#define SYSTEM_CAMERA_H

#include <iostream>

#include "../message_bus.h"
#include "../entity_pool.h"
#include "../components/camera.h"
#include "../components/pos_rot_scale.h"

class CameraUpdater : public System {
public:
    CameraUpdater(MessageBus* msg_bus);

    ~CameraUpdater();

    void handle_message(message_t msg);

private:
    void update_move(float forward, float right);
    void update_look(float dx, float dy);
    void update_1st_person(float forward, float right, float dx, float dy);
    void jump();

    std::string camera_style;

    Entity* camera;
    bool moving;
    pos_rot_scale_t* player_prs;
};

#endif // include guard
