#include <iostream>
#include <math.h>

#include <glm/gtx/string_cast.hpp>

#include "basic_geom.h"
#include "mesh.h"

Mesh make_cube() {
    float verts[] = {
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
    std::vector<unsigned int> indices = {
        2, 1, 0,    2, 0, 3,
        7, 6, 5,    7, 5, 4,
        11, 10, 9,  11, 9, 8,
        15, 14, 13, 15, 13, 12,
        19, 18, 17, 19, 17, 16, 
        23, 22, 21, 23, 21, 20
    };
    std::vector<glm::vec3> normals = {
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

void make_sphere(float radius, unsigned int lat_divisions, unsigned int log_divisions) {
    std::vector<vertex_t> vertices;
    // poles are special vertices. insert (manualy) one before and after the loop
    vertices.push_back({glm::vec3(0, -radius, 0), glm::vec3(0, -1, 0), glm::vec2(0, 0)});
    for(unsigned int i = 1; i < lat_divisions - 1; i++) {
        float phi = -M_PI + (i * 2 * M_PI / (float)lat_divisions);
        for(unsigned int j = 0; j < log_divisions; j++) {
            float theta = i * 2 * M_PI / (float)log_divisions;

            glm::vec3 pos(radius * cos(phi) * cos(theta),
                          radius * sin(phi),
                         -radius * cos(phi) * sin(theta)
            );
            glm::vec3 normal = (1 / radius) * pos;
            vertices.push_back({pos, normal, glm::vec2(0, 0)});
        }
    }
    vertices.push_back({glm::vec3(0, radius, 0), glm::vec3(0, 1, 0), glm::vec2(0, 0)});


    //TODO: use % go to 0 1 2 3 0 1 2 ... etc. all these special cases are stupid
    std::vector<unsigned int> indices;
    // bottom cap of sphere
    for(unsigned int i = 1; i < log_divisions; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i);
    }
    indices.push_back(0); indices.push_back(log_divisions); indices.push_back(1);
    for(unsigned int i = 1; i < vertices.size() - 1; i++) {
        indices.push_back(i);
    }
    // top cap of sphere
    for(unsigned int i = vertices.size() - 1 - log_divisions; i < vertices.size() - 2; i++) {
        indices.push_back(vertices.size() - 1);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(vertices.size() - 1); indices.push_back(vertices.size() - 1 - log_divisions); indices.push_back(vertices.size() - 2);

}
