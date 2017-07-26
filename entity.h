#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>

#include "message_bus.h"

class Entity {
public:
    Entity() {};

    ~Entity() {};

    std::vector<Entity> children;
};

#endif // include guard
