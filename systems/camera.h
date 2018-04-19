#ifndef SYSTEM_CAMERA_H
#define SYSTEM_CAMERA_H

#include <iostream>

#include "../message_bus.h"
#include "../entity_pool.h"
#include "../components/camera.h"

class CameraUpdater : public System {
public:
    CameraUpdater(MessageBus* msg_bus, Entity* camera) :
        System(msg_bus), camera(camera) {};

    ~CameraUpdater() {std::cout << "CameraUpdate DESTRUCTOR" << std::endl;};

    void handle_message(message_t msg);

private:
    void update_1st_person(float forward, float right, float dx, float dy);

    Entity* camera;
};

#endif // include guard
