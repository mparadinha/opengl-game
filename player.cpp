#include <glm/gtx/string_cast.hpp>

#include "player.h"
#include "signals.h"

void Player::handle_message(message_t msg) {
    switch(msg.code) {
        case NEW_FRAME:
            dt = msg.data[0];
            break;
        case PLAYER_MOVE:
            move(msg.data[0], msg.data[1]);
            break;
    }
}

void Player::move(float forward, float right) {
    auto diff = dt * speed * (forward * glm::vec3(0, 0, -1) + right * glm::vec3(1, 0, 0));
    position += diff;
    player_obj->translate(diff);
    camera->player_pos = position + glm::vec3(0, 2, 0);
}
