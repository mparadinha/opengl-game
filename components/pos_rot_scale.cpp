#include "pos_rot_scale.h"

glm::mat4 model_matrix(pos_rot_scale_t* prs) {
    glm::mat4 translation = glm::translate(glm::mat4(1), prs->pos);
    glm::mat4 scale = glm::scale(glm::mat4(1), prs->scale);
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(prs->pitch), {1, 0, 0});
    rotation = glm::rotate(glm::mat4(1), glm::radians(prs->yaw), {0, 1, 0});
    rotation = glm::rotate(glm::mat4(1), glm::radians(prs->roll), {0, 0, 1});

    return rotation * scale * translation;
}
