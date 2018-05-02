#include <vector>
#include <iostream>
#include <bitset>

#include "entity_pool.h"

unsigned int EntityPool::add_entity(Entity* e) {
    entities.push_back(e);
    return entities.size() - 1;
}

void EntityPool::add_special(Entity* e, ComponentType type) {
    specials[type] = e;
}

Entity* EntityPool::get_special(ComponentType type) {
    if(specials.find(type) == specials.end()) {
        return nullptr;
    }
    else {
        return specials[type];
    }
}

std::vector<Entity*> EntityPool::query(unsigned int mask) {
    std::vector<Entity*> result;
    for(Entity* e : entities) {
        if((mask & e->bitset) == mask) {
            result.push_back(e);
        }
    }

    return result;
}
