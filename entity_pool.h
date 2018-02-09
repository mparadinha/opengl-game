#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H

#include "entity.h"

class EntityPool {
public:
    ~EntityPool();

    void new_entity(Entity* e);
    Entity* new_entity(std::vector<Component*> list);
    std::vector<Entity*> request(std::string selection);

private:
    std::vector<Entity*> pool;
};

#endif // include guard
