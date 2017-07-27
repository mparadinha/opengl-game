#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "model.h"
#include "shader.h"
#include "camera.h"

class Player : public System {
public:
<<<<<<< HEAD
    Player(MessageBus* msg_bus, std::string model_path, Camera* camera);
=======
    Player(MessageBus* msg_bus, std::string model_path, Camera* camera) : System(msg_bus), model(Model(model_path)), camera(camera) {};
>>>>>>> b5c8cdb2c781d3ccbaaee45fd1b9b17e34a3ff31

    void handle_message(message_t msg);
    void render(Shader shader);

<<<<<<< HEAD
    ~Player();
=======
    ~Player() {};
>>>>>>> b5c8cdb2c781d3ccbaaee45fd1b9b17e34a3ff31

private:
    void update(float dt);

    glm::vec3 velocity;
    glm::vec3 position;
    glm::mat4 model_matrix;

<<<<<<< HEAD
    float speed = 2.0f;

=======
>>>>>>> b5c8cdb2c781d3ccbaaee45fd1b9b17e34a3ff31
    Model model;
    Camera* camera;
};

#endif // include guard
