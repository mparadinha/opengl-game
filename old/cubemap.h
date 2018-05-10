#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

#include <glm/glm.hpp>

#include "shader.h"
#include "entity_pool.h"
#include "components/camera.h"

class CubeMap {
public:
    CubeMap(std::string path, std::string extension);

    void set_uniforms(Shader& shader, Entity& camera);
    void render();

    ~CubeMap() {};

private:
    unsigned int vao, vbo;
    unsigned int id;
};

#endif // include guard
