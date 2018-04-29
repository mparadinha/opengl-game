#ifndef COMPONENT_MESH_H
#define COMPONENT_MESH_H

#define MAX_VBOS 5

struct mesh_t {
    // only the vao and num_indices need to be stored
    // in order to render the mesh, but we need to keep
    // all the allocated GPU buffers to clean them later
    unsigned int vao, vbos[MAX_VBOS], ebo;
    unsigned int num_indices;
    unsigned int index_data_type; // uint, byte, ubyte, etc...
};

#endif // include guard
