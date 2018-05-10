#include <iostream>

#include <glad.h>
#include <glm/glm.hpp>

#include "axes.h"

#include "../components/camera.h"

Axes::Axes() {    
    shader = new Shader("axes");

    std::vector<glm::vec3> verts = {
        glm::vec3(0, 0, 0), glm::vec3(0.2, 0, 0),
        glm::vec3(0, 0, 0), glm::vec3(0, 0.2, 0),
        glm::vec3(0, 0, 0), glm::vec3(0, 0, 0.2)
    };
    std::vector<glm::vec3> colors = {glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)};
    indices = {0, 1, 2, 3, 4, 5};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo); glGenBuffers(1, &vboc);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), &verts[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glBindBuffer(GL_ARRAY_BUFFER, vboc);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

Axes::~Axes() {
    delete shader;
}

void Axes::render(Entity* camera) {
    shader->use();
    camera_t* c_info = (camera_t*) camera->components[CAMERA];
    shader->set_uniform("view", glm::mat4(glm::mat3(c_info->view)));
    glBindVertexArray(vao);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);
}
