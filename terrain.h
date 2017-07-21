#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>

#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>

#include "shader.h"
#include "mesh.h"

class Terrain {
public:
    Terrain(float size, unsigned int density);

    void render();

    ~Terrain() {};

private:
    std::vector<Mesh> meshes;
};

#endif // include guard
