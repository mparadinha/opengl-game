#ifndef ENTITY_H
#define ENTITY_H

#include <map>
#include <vector>

#include "components.h"

struct Entity {
    Entity(std::vector<Component*> comp_list);
    ~Entity();

    void add_component(std::string handle, Component* comp);


    std::map<std::string, Component*> components;
};

#endif // include guard
