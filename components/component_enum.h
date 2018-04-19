#ifndef COMPONENT_ENUM_H
#define COMPONENT_ENUM_H

enum ComponentType {
    TRANSFORMATION = 0x1,
    MESH = 0x2,
    ANIMATION = 0x4,
    CAMERA = 0x8,
    POS_ROT_SCALE = 0xa
};

struct Component {
    const static ComponentType type;

    Component() {};
    virtual ~Component() {};
};

#endif // include guard
