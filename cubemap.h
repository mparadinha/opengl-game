#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

#include <glm/glm.hpp>

#include "shader.h"
#include "camera.h"

class CubeMap {
public:
    CubeMap(std::string path, std::string extension);

    void set_uniforms(Shader& shader, Camera camera);
    void render();

    ~CubeMap() {};

private:
    unsigned int vao, vbo;
    unsigned int id;
};

#endif // include guard
