#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "../entity_pool.h"
#include "component_enum.h"

struct transformation_t : public Component {
    const ComponentType type = TRANSFORMATION;

    glm::mat4 scaling, translation, rotation;

    void rotate(float angle, glm::vec3 axis) {
        rotation = glm::rotate(rotation, glm::radians(angle), axis);
    }
    void translate(glm::vec3 diff) {
        translation = glm::translate(translation, diff);
    }
    void scale(glm::vec3 scalar) {
        scaling = glm::scale(scaling, scalar);
    }
    void scale(float scalar) {
        scaling = glm::scale(scaling, glm::vec3(scalar));
    }

    glm::mat4 get_model() { return rotation * translation * scale; }
};

#endif // include guard
