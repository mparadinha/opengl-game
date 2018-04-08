#ifndef MESH_T_H
#define MESH_T_H

#include "../entity_pool.h"
#include "component_enum.h"

#define MAX_VBOS 5

struct mesh_t {
    const static ComponentType type = MESH;

    // only the vao and num_indices need to be stored
    // in order to render the mesh, but we need to keep
    // all the allocated GPU buffers to clean them later
    unsigned int vao, vbos[MAX_VBOS], ebo;
    unsigned int num_indices;
    unsigned int index_data_type; // uint, byte, ubyte, etc...
};

#endif // include guard
