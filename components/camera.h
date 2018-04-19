#ifndef COMPONENT_CAMERA_H
#define COMPONENT_CAMERA_H

#include <glm/glm.hpp>

#include "component_enum.h"

struct camera_t {
    const static ComponentType type = CAMERA;

    glm::mat4 view, projection;
    float fov;
};

#endif // include guard
