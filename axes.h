#ifndef AXES_H
#define AXES_H

#include "shader.h"
#include "entity_pool.h"

class Axes {
public:
    Axes();
    ~Axes();

    void render(Entity* camera);

private:
    unsigned int vao, vbo, vboc, ebo;
    Shader* shader;

    std::vector<unsigned int> indices;
};

#endif
