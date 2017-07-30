#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <cstddef> // for using intermidiate vars of size_t type

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef struct {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
} vertex_t;

typedef struct {
    unsigned int id;
    std::string type;
} texture_t;

// TODO: move the textures parts to the model class
// mesh is only the geometry
class Mesh {
public:
    Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices);

    void render();

    ~Mesh();

private:
    void setup();
    void set_vertex_attribute(int index, int size, size_t stride);

    std::vector<vertex_t> vertices; // might need it for later
    std::vector<unsigned int> indices;

    unsigned int vao, vbo, ebo;
};

#endif // include guard
