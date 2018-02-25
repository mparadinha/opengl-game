#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "../message_bus.h"

class Animator : public System {
public:
    Animator(MessageBus* msg_bus);
    ~Animator();

private:

};

#endif // include guard
