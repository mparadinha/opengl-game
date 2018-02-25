#ifndef MESH_T_H
#define MESH_T_H

#include "../entity_pool.h"
#include "component_enum.h"

struct mesh_t {
    const static ComponentType type = MESH;

    unsigned int vao, vbo, ebo;
    unsigned int num_indices;
};

#endif // include guard
