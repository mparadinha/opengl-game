#include <cstddef> // for using intermidiate vars of size_t type
#include <iostream>

#include <glad.h>

#include "mesh.h"

Mesh::Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices)
: vertices(vertices), indices(indices) {
    // create all the buffers for vertices and stuff
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // setup and fill the vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_t),
        &vertices[0], GL_STATIC_DRAW);
    // same for ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    // now tell the GPU what data represents what
    set_vertex_attribute(0, 3, offsetof(vertex_t, position)); // positions
    set_vertex_attribute(1, 3, offsetof(vertex_t, normal)); // normals
    set_vertex_attribute(2, 2, offsetof(vertex_t, texcoords)); // texture coords
    set_vertex_attribute(3, 3, offsetof(vertex_t, bone_ids));
    set_vertex_attribute(4, 3, offsetof(vertex_t, weights));
}

Mesh::~Mesh() {}

void Mesh::render() {
    // tell opengl to actually call the shaders to draw everything
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0); <- its just a waste of bind calls since every other
    // function that draw should call its respective bind before
}

void Mesh::set_vertex_attribute(int index, int size, size_t stride, GLenum type) {
    glEnableVertexAttribArray(index);
    if(type == GL_FLOAT) {
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
            (void*) stride);
    }
    else if(type == GL_INT) { 
        glVertexAttribIPointer(index, size, GL_INT, sizeof(vertex_t),
            (void*) stride);
    }
}
