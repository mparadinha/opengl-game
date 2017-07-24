#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>

#include <glm/gtx/string_cast.hpp>
#if __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include "glad.h"
#endif

#include "quad.h"
#include "texture.h"

class Terrain {
public:
    Terrain() {
        quads.push_back(Quad(100, 100));
        quads.push_back(Quad(100, 100, -100, -100));
        quads.push_back(Quad(100, 100, -100, 0));
        quads.push_back(Quad(100, 100, 0, -100));

        Texture tex = Texture("res/green-grass-texture.jpg");
        texture_id = tex.load();
    }

    void render() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        for(Quad quad : quads) quad.render();
    }

    ~Terrain() {};

private:
    int texture_id;
    std::vector<Quad> quads;
};

#endif // include guard
