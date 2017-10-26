#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "components.h"

struct Transformation : Component {
    const static name = "transformation";
    Transformation() { position = glm::vec3(0); rotation = glm::vec4(0); scale(1); }

    glm::mat4 model() {
        glm::mat4 m;
        m = glm::translate(m, position);
        m = glm::rotate(m, glm::radians(rotation[3]),
            glm::vec3(rotation[0], rotation[1], rotation[2]));
        m = glm::scale(m, scale);
        return m;
    }

    glm::vec3 position;
    glm::vec4 rotation;
    glm::vec3 scale;

};

struct Drawable : Component {
    // deps: transformation
    const static name = "drawable";
    Drawable(std::string file) : model(Model(file));

    Model model;
};

// first person camera
struct CameraFP : Component {
    // deps: transformation
    const static name = "camera_fp";
};
// 3rd person camera
struct CameraTP : Component {
    // deps: transformation
    const static name = "camera_tp";
};
