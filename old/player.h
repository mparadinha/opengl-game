#ifndef PLAYER_H
#define PLAYER_H

#include "game_object.h"
#include "camera.h"
#include "message_bus.h"

class Player : public System {
public:
    Player(MessageBus* msg_bus, GameObject* player_obj, Camera* camera, glm::vec3 position) :
        System(msg_bus), player_obj(player_obj), camera(camera), position(position) {};

    void render(Shader& shader);

    ~Player() {};

    void handle_message(message_t msg);

private:
    void move(float forward, float right);

    GameObject* player_obj;
    Camera* camera;
    glm::vec3 position;
    float dt;

    float speed = 3.0f;
};

#endif // include guard
