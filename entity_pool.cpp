#include <vector>
#include <iostream>
#include <bitset>

#include "entity_pool.h"

unsigned int EntityPool::add_entity(Entity* e) {
    entities.push_back(e);
    return entities.size() - 1;
}

std::vector<Entity*> EntityPool::query(unsigned int mask) {
    std::vector<Entity*> result;
    for(Entity* e : entities) {
        if((mask & e->bitset) == mask)
            result.push_back(e);
    }

    return result;
}
