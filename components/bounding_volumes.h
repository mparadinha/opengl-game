#ifndef COMPONENT_BOUDING_VOLUMES_H
#define COMPONENT_BOUDING_VOLUMES_H

#include <glm/glm.hpp>

struct aabb_t {
    glm::vec3 center;
    glm::vec3 size; // halfwidth distance in each of the axes    
};

struct sphere_bv_t {
    glm::vec3 center;
    float radius;
};

#endif
