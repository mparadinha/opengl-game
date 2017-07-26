#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "model.h"
#include "shader.h"
#include "camera.h"

class Player : public System {
public:
    Player(MessageBus* msg_bus, std::string model_path, Camera* camera);

    void handle_message(message_t msg);
    void render(Shader shader);

    ~Player();

private:
    void update(float dt);

    glm::vec3 velocity;
    glm::vec3 position;
    glm::mat4 model_matrix;

    float speed = 2.0f;

    Model model;
    Camera* camera;
};

#endif // include guard
