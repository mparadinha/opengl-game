#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <map>

#include "components.h"

class Entity {
public:
    Entity();
    Entity(std::map<std::string, Component*> comp_list);

    void add_component(std::string handle, Component* comp);

private:
    std::map<std::string, Component*> components;
};

#endif // include guard
