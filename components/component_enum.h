#ifndef COMPONENT_ENUM_H
#define COMPONENT_ENUM_H

enum ComponentType {
    TRANSFORMATION = 0x1,
    MESH = 0x2,
    ANIMATION = 0x4
};

struct Component {
    const static ComponentType type;

    Component() {};
    virtual ~Component() {};
};

#endif // include guard
