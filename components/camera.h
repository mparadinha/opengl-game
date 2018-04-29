#ifndef COMPONENT_CAMERA_H
#define COMPONENT_CAMERA_H

#include <glm/glm.hpp>

struct camera_t {
    glm::mat4 view, projection;
    float fov;
};

#endif // include guard
