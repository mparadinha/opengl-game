#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"

struct Entity; // sort of a foward declaration because of the circular dependency

struct Component {
    const std::string name;
    void bind(Entity* master) { this->master = master; };

private:
    Entity* master;
};

struct Transformation : Component {
    glm::vec3 position, scale;
    glm::vec4 rotation;

    const std::string name = "transformation";
    Transformation() {
        position = glm::vec3(0); rotation = glm::vec4(0); scale = glm::vec3(1);
    }
    glm::mat4 model() {
        glm::mat4 m;
        m = glm::translate(m, position);
        m = glm::rotate(m, glm::radians(rotation[3]),
            glm::vec3(rotation[0], rotation[1], rotation[2]));
        m = glm::scale(m, scale);
        return m;
    }
};

struct Drawable : Component {
    Model model;

    const std::string name = "drawable";
    Drawable(std::string file) : model(Model(file)) {}
    void draw() { model.render(); }
};

struct _Camera : Component {
    void update(float dx, float dy);
};

struct CameraFP : Component {
    const std::string name = "camera_fp";
};

struct CameraTP : Component {
    const std::string name = "camera_tp";
};

#endif // include guard
