#ifndef POSROTSCALE_COMP_H
#define POSROTSCALE_COMP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "component_enum.h"

struct pos_rot_scale_t {
    const static ComponentType type = POS_ROT_SCALE;
    glm::vec3 pos, scale;
    float yaw, pitch, roll;
};

glm::mat4 model_matrix(pos_rot_scale_t* prs);

#endif // include guard
