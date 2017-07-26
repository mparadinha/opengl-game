#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "model.h"
#include "shader.h"
#include "camera.h"

class Player : public System {
public:
    Player(MessageBus* msg_bus, std::string model_path, Camera* camera) : System(msg_bus), model(Model(model_path)), camera(camera) {};

    void handle_message(message_t msg);
    void render(Shader shader);

    ~Player() {};

private:
    void update(float dt);

    glm::vec3 velocity;
    glm::vec3 position;
    glm::mat4 model_matrix;

    Model model;
    Camera* camera;
};

#endif // include guard
