#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H

#include <map>
#include <vector>

#include "components/component_enum.h"

struct Entity {
    unsigned int bitset;
    std::map<ComponentType, void*> components;
};

class EntityPool {
public:
    EntityPool() {};
    ~EntityPool() {};

    unsigned int add_entity(Entity* e);

    void add_special(Entity* e, ComponentType type);
    Entity* get_special(ComponentType type);

    std::vector<Entity*> query(unsigned int mask);

private:
    std::vector<Entity*> entities;
    std::map<ComponentType, Entity*> specials;
};

#endif // include guard
