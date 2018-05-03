#ifndef QUAD_H
#define QUAD_H

#include "mesh.h"

class Quad : public Mesh {
public:
    // TODO: allow for non square quad
    Quad(float size, unsigned int density, float startx = 0, float startz = 0) :
        Mesh(get_vertices(size, density, startx, startz), get_indices(density)) {};

    ~Quad() {};

private:
    // c++ cant call the parent constructor inside the child constructor 
    // only in the init list, so FUCK IT i'll just do this
    std::vector<vertex_t> get_vertices(float size, unsigned density, float startx, float startz) {
        std::vector<vertex_t> verts;
        float diff = size / (density - 1);
        for(unsigned int z = 0; z < density; z++) {
            for(unsigned int x = 0; x < density; x++) {
                vertex_t vertice = {
                    glm::vec3(startx + x * diff, 0, startz + z * diff), // position
                    glm::vec3(0, 1, 0), // normal
                    glm::vec2(z / (float)(density - 1), x / (float) density - 1), // texture coordinates
                };
                verts.push_back(vertice);
            }
        }

        return verts;
    };

    std::vector<unsigned int> get_indices(unsigned int density) {
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

        return indices;
    };
};

#endif
