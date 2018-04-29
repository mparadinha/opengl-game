#ifndef POSROTSCALE_COMP_H
#define POSROTSCALE_COMP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//TODO: remove this and just use rigid_body components everywhere instead
// or maybe not because this might be usefull for drawing bounding boxes

struct pos_rot_scale_t {
    glm::vec3 pos, scale;
    float yaw, pitch, roll;
};

glm::mat4 model_matrix(pos_rot_scale_t* prs);

#endif // include guard
