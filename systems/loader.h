#ifndef LOADER_H
#define LOADER_H

#include "../message_bus.h"

class Loader : public System {
public:
    Loader(MessageBus* msg_bus);
    ~Loader();

private:

};

#endif // include guard
