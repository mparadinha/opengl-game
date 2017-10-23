#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H

#include "entity.h"

class EntityPool {
public:
    EntityPool();

    ~EntityPool();

private:
    std::vector<Entity> pool;
};

#endif // include guard
