#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H

#include "entity.h"
#include "components.h"

// base class for all entities
class Entity {
private:
    std::vector<Component> components;
};

class EntityPool {
public:
    EntityPool();

    ~EntityPool();

private:
    std::vector<Entity> pool;
};

#endif // include guard
