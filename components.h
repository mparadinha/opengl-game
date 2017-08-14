#ifndef COMPONENTS_H
#define COMPONENTS_H

class Entity; // so the Component class knows about entity
// and avoids circular dependency madness

class Component {
public:
    Entity* master;
};

#endif // include guard
