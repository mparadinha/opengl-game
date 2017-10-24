#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "components.h"

struct PosRotScale {
    glm::vec3 position;
    glm::vec4 rotation;
    glm::vec3 scale;

    glm::mat4 model() {
        glm::mat4 m;
        m = glm::translate(m, position);
        m = glm::rotate(m, glm::radians(rotation[3]),
            glm::vec3(rotation[0], rotation[1], rotation[2]));
        m = glm::scale(m, scale);
        return m;
    }
};
