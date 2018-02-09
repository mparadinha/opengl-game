#include <vector>
#include <iostream>

#include "entity.h"

Entity::Entity(std::vector<Component*> comp_list) {
    for(auto c : comp_list) add_component(c->name, c);
}

Entity::~Entity() {
    for(auto c : components) {
        std::cout << "deleting: " << (c.second)->name << std::endl;
        delete c.second;
    }
}

void Entity::add_component(std::string name, Component* c) {
    c->bind(this);
    components.insert({name, c});
}
