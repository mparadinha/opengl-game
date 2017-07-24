#include <cstddef> // for using intermidiate vars of size_t type
#include <iostream>

#include "glad.h"

#include "mesh.h"

void Mesh::setup() {
    // create all the buffers for vertices and stuff
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // setup the vbo for later filling
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_t),
        &vertices[0], GL_STATIC_DRAW);
    // same for ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    // now fill in the vertex data in the GPU
    set_vertex_attribute(0, 3, offsetof(vertex_t, position)); // positions
    set_vertex_attribute(1, 3, offsetof(vertex_t, normal)); // normals
    set_vertex_attribute(2, 2, offsetof(vertex_t, texcoords)); // texture coords
}

void Mesh::render() {
    for(unsigned int i = 0; i < textures.size(); i++) {
        // specify what texture unit we are using (one for each texture)
        // and then just "sync" the shader uniform and bind the gl "object"
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // finaly tell opengl to actually call the shaders to draw everything
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0); <- its just a waste of bind calls since every other
    // function that draw should call its respective bind before
}

void Mesh::set_vertex_attribute(int index, int size, size_t stride) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
        (void*) stride);
}
