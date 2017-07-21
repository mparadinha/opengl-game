#include <iostream>

#include "terrain.h"
#include "mesh.h"

#include <glm/gtx/string_cast.hpp>

// TODO: this "terrain class is just a quad actualy
// so we should move it to a simple quad class
// and make the terrain use one or more quad as well as textures and generating the height
Terrain::Terrain(float size, unsigned int density) {
    std::vector<vertex_t> verts;
    float half = size / 2.0f;
    float diff = size / (density - 1);
    for(unsigned int z = 0; z < density; z++) {
        for(unsigned int x = 0; x < density; x++) {
            vertex_t vertice = {
                glm::vec3(-half + x * diff, 0, -half + z * diff), // position
                glm::vec3(0, 1, 0), // normal
                glm::vec2(x * diff, z * diff), // tex coordinates
            };
        verts.push_back(vertice);
        }
    }

    std::vector<unsigned int> indices;
    for(unsigned int z = 0; z < density - 1; z++) {
        for(unsigned int x = 0; x < density - 1; x++) {
            // NOTE: carefull with the winding order for back face vertex culling
            indices.push_back(x + z * density);
            indices.push_back(x + 1 + (z + 1) * density);
            indices.push_back(x + 1 + z * density);
    
            indices.push_back(x + z * density);
            indices.push_back(x + (z + 1) * density);
            indices.push_back(x + 1 + (z + 1) * density);
        }
    }

    meshes.push_back(Mesh(verts, indices, std::vector<texture_t>()));
}

void Terrain::render() {
    meshes[0].render();
}
