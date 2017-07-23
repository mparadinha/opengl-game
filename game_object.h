#ifndef GAME_OBJ_H
#define GAME_OBJ_H

#include <string>
#include <vector>

#if __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "shader.h"
#include "model.h"

class GameObject {
public:
    GameObject(std::string model_file) : model(Model(model_file)) {};

    // send draw calls to openGL
    void render(Shader shader) {
        set_all_uniforms(shader);
        model.render();
    }

    // uniform setting
    void set_all_uniforms(Shader shader) {
        set_model_uniform(shader);
        set_normal_uniform(shader);
    }
    void set_model_uniform(Shader shader) {
        shader.set_uniform("model", &model_matrix[0][0]);
    }
    void set_normal_uniform(Shader shader) {
        glm::mat3 inverse = glm::inverseTranspose(glm::mat3(model_matrix));
        shader.set_uniform("normal_matrix", &inverse[0][0]);
    }

    // transformation functions
    void rotate(float angle, glm::vec3 axis) {
        model_matrix = glm::rotate(model_matrix, glm::radians(angle), axis);
    }
    void translate(glm::vec3 diff) {
        model_matrix = glm::translate(model_matrix, diff);
    }
    void scale(glm::vec3 scalar) {
        model_matrix = glm::scale(model_matrix, scalar);
    }

    ~GameObject() {};

private:
    glm::mat4 model_matrix;
    Model model;
};

#endif // include guard 
