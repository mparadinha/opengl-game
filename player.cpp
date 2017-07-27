#include <glm/gtc/matrix_transform.hpp>

#include "signals.h"
#include "shader.h"
#include "player.h"

Player::Player(MessageBus* msg_bus, std::string model_path, Camera* camera)
    : System(msg_bus), model(Model(model_path)), camera(camera) {
    model_matrix = glm::translate(model_matrix, glm::vec3(0, 2, 0));
}

Player::~Player() {}

void Player::render(Shader shader) {
    shader.set_uniform("model", glm::value_ptr(model_matrix));
    model.render();
}

void Player::handle_message(message_t msg) {
    switch(msg.code) {
    case NEW_FRAME: update(msg.data[0]); break;
    case PLAYER_MOVE:
        //TODO: rotate the vector
        // update velocity vector
        velocity = speed * glm::vec3(0, 0, -msg.data[0]);
    }
}
    
void Player::update(float dt) {
    //TODO: when actual movemetnt occurs we need to rotate the model as well

    // move position and model
    position += dt * velocity;
    model_matrix = glm::translate(model_matrix, dt * velocity);

    // update the camera player_pos
    camera->player_pos = position;
}
