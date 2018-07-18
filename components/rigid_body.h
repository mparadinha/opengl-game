#ifndef COMPONENT_RIGID_BODY_H
#define COMPONENT_RIGID_BODY_H

struct rigid_body_t {
    glm::vec3 pos, vel;
    glm::vec3 scale;
    float yaw, pitch, roll;

    float inv_mass;
    bool floating = false;
};

#endif
