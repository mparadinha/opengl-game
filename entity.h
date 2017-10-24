#ifndef ENTITY_H
#define ENTITY_H

#include <map>
#include <vector>

#include "components.h"

class Entity {
public:
    Entity(std::vector<Component*> comp_list);

    void add_component(std::string handle, Component* comp);

    ~Entity();

private:
    std::map<std::string, Component*> components;
};

#endif // include guard
