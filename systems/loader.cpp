#include <iostream>
#include <fstream>

#include <glad.h>

#include "loader.h"
#include "../gltf.h"
#include "../signals.h"
#include "../components/mesh.h"
#include "../components/animation.h"

Loader::Loader(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing loader..." << std::endl;
}

Loader::~Loader() {}

void Loader::handle_message(message_t msg) {
    switch(msg.code) {
    }
}

mesh_t Loader::load_mesh(std::string filepath) {
    //TODO: need to parse and include in the mesh_t the type of data that
    // the indices are. previous was because they were sent as single byte
    // uints and glDrawElements used GL_UNSIGNED_INT instead of GL_UNSIGNED_BYTE
    std::cout << "loading mesh from " << filepath << std::endl;
    std::string file_dir = filepath.substr(0, filepath.find_last_of("/"));
    gltf::file_t file(filepath);

    mesh_t out_mesh; // this is the "engine" mesh that will be used later

    //TODO: support more than one mesh/primitive/buffer
    gltf::primitive_t primitive = file.meshes[0].primitives[0];
    gltf::uri_file_t buffer(file_dir + "/" + file.buffers[0].uri, file.buffers[0].byte_length);

    // create and bind the vao
    glGenVertexArrays(1, &out_mesh.vao);
    glBindVertexArray(out_mesh.vao);

    // load the mesh attributes each into its own vbo
    load_vbo(file, buffer, primitive.attributes["POSITION"], out_mesh.vbos, 0, 3);
    load_vbo(file, buffer, primitive.attributes["NORMAL"], out_mesh.vbos, 1, 3);

    // create, bind, and fill ebo
    gltf::accessor_t indices_accessor = file.accessors[primitive.indices];
    glGenBuffers(1, &out_mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_mesh.ebo);
    read_buffer_data(buffer, file.buffer_views[indices_accessor.buffer_view], GL_ELEMENT_ARRAY_BUFFER);
    // and store the number of indices (needed for drawing)
    out_mesh.num_indices = file.accessors[primitive.indices].count;
    //TODO: unhack this. make a map/table with all the gltf values and corresponding GL types. the hardcoded 5121 is for unsigned bytes
    out_mesh.index_data_type = (indices_accessor.component_type == 5121) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_INT;

    return out_mesh;
}

void load_vbo(gltf::file_t file, gltf::uri_file_t& buffer, unsigned int accessor_idx, unsigned int vbos[], unsigned int vbo_idx, unsigned int size) {
    // create and bind vbo
    glGenBuffers(1, &vbos[vbo_idx]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[vbo_idx]);

    gltf::accessor_t accessor = file.accessors[accessor_idx];
    gltf::buffer_view_t buf_view = file.buffer_views[accessor.buffer_view];

    read_buffer_data(buffer, buf_view, GL_ARRAY_BUFFER);

    // set the attributes for communicating with the shaders 
    glEnableVertexAttribArray(vbo_idx); // in shader: "layout(location = <vbo_idx>)"
    glVertexAttribPointer(vbo_idx,
        size, /* number of components per element of the vbo (e.g. pos=3, texcoord=2) */
        GL_FLOAT,
        GL_FALSE, /* don't normalize data */
        0, /* stride */
        (void*) 0); /* pointer to first component of attribute in vbo */

    std::cout << std::endl;
}

void read_buffer_data(gltf::uri_file_t& buffer, gltf::buffer_view_t buf_view, unsigned int data_type) {
    // put the buffer file pointer pointing to where the data is
    std::vector<unsigned char> data = buffer.read<unsigned char>(
        buf_view.byte_offset, buf_view.byte_length);

    // load data from binary file to GPU
    glBufferData(data_type, /* GL_ARRAY_BUFFER for verts/normals GL_ELEMENT_BUFFER for indices */
        buf_view.byte_length, /* size in bytes */
        &data[0], /* pointer to data */
        GL_STATIC_DRAW);
}
