#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H

#include <map>
#include <vector>

#include "components/component_enum.h"

struct Component;

struct Entity {
    unsigned int bitset;
    std::map<ComponentType, Component> components;
};

class EntityPool {
public:
    EntityPool();
    ~EntityPool();

    unsigned int add_entity(Entity* e);
    std::vector<Entity*> query(unsigned int mask);

private:
    std::vector<Entity*> entities;
};

#endif // include guard
