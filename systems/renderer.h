#ifndef RENDERER_H
#define RENDERER_H

#include "../message_bus.h"

class Renderer : public System {
public:
    Renderer(MessageBus* msg_bus);
    ~Renderer();

private:

};

#endif // include guard
