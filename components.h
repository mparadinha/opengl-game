#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>

class Entity; // sort of a foward declaration because of the circular dependency

struct Component {
    const std::string name;
    void bind(Entity* master) { this->master = master; };

private:
    Entity* master;
};

#endif // include guard
