#include <iostream>

#include "animator.h"

Animator::Animator(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing animator..." << std::endl;
}

Animator::~Animator() {}
