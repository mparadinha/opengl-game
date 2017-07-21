#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>

#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>

#include "quad.h"

class Terrain {
public:
    Terrain() {
        quads.push_back(Quad(50, 50));
        quads.push_back(Quad(50, 50, -50, -50));
        quads.push_back(Quad(50, 50, -50, 0));
        quads.push_back(Quad(50, 50, 0, -50));
    }

    void render() {
        for(Quad quad : quads) quad.render();
    }

    ~Terrain() {};

private:
    std::vector<Quad> quads;
};

#endif // include guard
