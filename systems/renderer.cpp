#include <iostream>

#include "renderer.h"

Renderer::Renderer(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing renderer..." << std::endl;
}

Renderer::~Renderer() {}
