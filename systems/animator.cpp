#include <iostream>

#include "animator.h"

Animator::Animator(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing animator..." << std::endl;
}

Animator::~Animator() {}

void Animator::handle_message(message_t msg) {
    switch(msg.code) {
    case NEW_FRAME:
        update_animations();
        break;
    }
}

void Animator::update_animations() {

}
