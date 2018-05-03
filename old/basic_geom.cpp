#include <iostream>
#include <math.h>

#include <glm/gtx/string_cast.hpp>

#include "basic_geom.h"
#include "mesh.h"

Mesh make_cube() {
    const static float verts[] = {
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
    
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
    
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
    
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
    
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f
    };
    const static std::vector<unsigned int> indices = {
        2, 1, 0,    2, 0, 3,
        7, 6, 5,    7, 5, 4,
        11, 10, 9,  11, 9, 8,
        15, 14, 13, 15, 13, 12,
        19, 18, 17, 19, 17, 16, 
        23, 22, 21, 23, 21, 20
    };
    const static std::vector<glm::vec3> normals = {
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, -1),
        glm::vec3(-1, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, -1, 0)
    };

    // make vector of positions
    std::vector<glm::vec3> positions;
    for(unsigned int i = 0; i < 3 * 4 * 6; i += 3) {
        positions.push_back(
            glm::vec3(verts[i], verts[i + 1], verts[i + 2])
        );
    }

    std::vector<vertex_t> vertices;
    for(unsigned int i = 0; i < positions.size(); i++) {
        vertex_t vertex = {
            positions[i], normals[i / 4], glm::vec2(0, 0)
        };
        vertices.push_back(vertex);
    }

    return Mesh(vertices, indices);
}

Mesh make_sphere(unsigned int divisions) {
    std::vector<glm::vec3> positions;
    for(unsigned int i = 0; i < divisions + 1; i++) {
        for(unsigned int j = 0; j < divisions + 1; j++) {
            float theta = (-M_PI / 2.0f) + (M_PI / ((float) divisions)) * i;
            float phi = (2 * M_PI / ((float) divisions)) * j;
            glm::vec3 pos(
                cos(theta) * sin(phi),
                sin(theta),
                cos(theta) * cos(phi)
            );
            positions.push_back(pos);
        }
    }

    std::vector<unsigned int> indices;
    unsigned int vertex = 0;
    for(unsigned int i = 0; i < divisions; i++) {
        for(unsigned int j = 0; j < divisions; j++) {
            // do square on vertex
            indices.insert(indices.end(), {vertex, vertex + divisions + 2, vertex + divisions + 1});
            indices.insert(indices.end(), {vertex, vertex + 1, vertex + divisions + 2});

            vertex++;
        }
        vertex++; // skip the last vertex of the line
    }

    std::vector<vertex_t> vertices;
    for(glm::vec3 p : positions) {
        vertices.push_back({p, p, glm::vec2(0, 0)});
    }
    return Mesh(vertices, indices);
}
