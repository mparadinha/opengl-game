#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../entity_pool.h"
#include "component_enum.h"

struct bone_transform_t {
    glm::vec3 position;
    glm::quat rotation;
};

struct key_frame_t {
    bone_transform_t bone_tranform;
    float time_stamp;
};

struct animation_t : public Component {
    const static ComponentType type = ANIMATION;

    float duration;
    std::vector<key_frame_t> key_frames;
};

#endif // include guard
