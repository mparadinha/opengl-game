#include <vector>

#include "entity.h"

Entity::Entity(std::vector<Component*> comp_list) {
    for(auto c : comp_list)
        add_component(c->name, c);
}

void Entity::add_component(std::string name, Component* c) {
    c->bind(this);
    components.insert({name, c});
}
