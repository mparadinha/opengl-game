#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.h"

// TODO: make child class for each type of light
// also investigate the <=> of calling super() so that each subclass can call a method without always
// using the "light." prefix

class Light {
public:
    Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
            float constant, float linear, float quadratic)
    : position(position), diffuse(diffuse), specular(specular),
            constant(constant), linear(linear), quadratic(quadratic) {};

    void set_all_uniforms(Shader shader) {
        shader.set_uniform("light.position", position);
        shader.set_uniform("light.ambient", ambient);
        shader.set_uniform("light.diffuse", diffuse);
        shader.set_uniform("light.specular", specular);
        shader.set_uniform("light.constant", constant);
        shader.set_uniform("light.linear", linear);
        shader.set_uniform("light.quadratic", quadratic);
    };

    ~Light() {};

private:
    glm::vec3 position;
    glm::vec3 ambient, diffuse, specular;
    float constant, linear, quadratic;
};

#endif // include guard
