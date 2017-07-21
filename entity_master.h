#ifndef ENTITY_MASTER_H
#define ENTITY_MASTER_H

#include <vector>

#include "game_object.h"
#include "message_bus.h"

class EntityMaster : public System {
public:
    EntityMaster() {};

    virtual void handle_message(message_t msg);

    ~EntityMaster() {};

private:
    std::vector<GameObject> entities;
};

#endif
