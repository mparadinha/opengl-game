#ifndef GAME_OBJ_H
#define GAME_OBJ_H

#include <string>
#include <vector>

#if __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <glad.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "message_bus.h"
#include "shader.h"
#include "model.h"

class GameObject {
public:
    GameObject(std::string model_file) : model(Model(model_file)) {};

    // send draw calls to openGL
    void render(Shader& shader) {
        set_all_uniforms(shader);
        model.render();
    }

    // uniform setting
    void set_all_uniforms(Shader& shader) {
        set_model_uniform(shader);
        set_normal_uniform(shader);
    }
    void set_model_uniform(Shader& shader) {
        model_matrix = rotation * translation * scaling;
        shader.set_uniform("model", &model_matrix[0][0]);
    }
    void set_normal_uniform(Shader& shader) {
        glm::mat3 inverse = glm::inverseTranspose(glm::mat3(model_matrix));
        shader.set_uniform("normal_matrix", &inverse[0][0]);
    }

    // transformation functions
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

    ~GameObject() {};

private:
    glm::mat4 rotation, translation, scaling;
    glm::mat4 model_matrix;
    Model model;
    std::vector<GameObject> children;
};

#endif // include guard 
