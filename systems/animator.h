#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "../message_bus.h"

class Animator : public System {
public:
    Animator(MessageBus* msg_bus);
    ~Animator();

    void handle_message(message_t msg);

private:
    void update_animations();
};

#endif // include guard
