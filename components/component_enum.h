#ifndef COMPONENT_ENUM_H
#define COMPONENT_ENUM_H

enum ComponentType {
    TRANSFORMATION = 1 << 0,
    MESH           = 1 << 1,
    ANIMATION      = 1 << 2,
    CAMERA         = 1 << 3,
    POS_ROT_SCALE  = 1 << 4,
    RIGID_BODY     = 1 << 5,
    AABB           = 1 << 6,
    TEXTURE        = 1 << 7,
    BB_CUBE        = 1 << 8,
    SKYBOX         = 1 << 9
};

#endif // include guard
