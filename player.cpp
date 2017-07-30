#include <glm/gtc/matrix_transform.hpp>

#include "signals.h"
#include "shader.h"
#include "player.h"

Player::Player(MessageBus* msg_bus, std::string model_path, Camera* camera)
    : System(msg_bus), model(Model(model_path)), camera(camera) {
    model_matrix = glm::scale(model_matrix, glm::vec3(0.7f, 0.7f, 0.7f));
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
        model_matrix = glm::rotate(model_matrix, glm::radians(-msg.data[1]), glm::vec3(0, 1, 0));
        // update velocity vector
        velocity = speed * glm::vec3(0, 0, -msg.data[0]);

        // rotation matrix for the velocity direction
        glm::mat4 rot(1); rot = glm::rotate(rot, glm::radians(-msg.data[1]), glm::vec3(0, 1, 0));
        vel_dir = glm::vec3(rot * glm::vec4(vel_dir, 1.0));
    }
}
    
void Player::update(float dt) {
    //TODO: when actual movemetnt occurs we need to rotate the model as well

    // move position and model
    //position += dt * glm::normalize(vel_dir) * glm::length(velocity);
    position += dt * velocity;
    model_matrix = glm::translate(model_matrix, dt * velocity);

    // update the camera player_pos
    camera->player_pos = position;
}
