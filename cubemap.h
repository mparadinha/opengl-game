#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

#include <glm/glm.hpp>

#include "shader.h"
#include "camera.h"

class CubeMap {
public:
    CubeMap(std::string path, std::string extension);

    void set_uniforms(Shader shader, Camera camera) {
        glm::mat4 view = glm::mat4(glm::mat3(camera.get_view()));
        shader.set_uniform("view", &view[0][0]);
        camera.set_projection_uniform(shader);
    }

    void render();

    ~CubeMap() {};

private:
    unsigned int vao, vbo;
    unsigned int id;
};

#endif // include guard
