#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <cstddef> // for using intermidiate vars of size_t type

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad.h>

typedef struct {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
    glm::vec3 bone_ids; // bones that affect this vertex
    glm::vec3 weights; // the weight each bone has on the vertex
} vertex_t;

typedef struct {
    unsigned int id;
    std::string type;
} texture_t;

class Mesh {
public:
    Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices);

    void render();

    ~Mesh();

private:
    void setup();
    void set_vertex_attribute(int index, int size, size_t stride, GLenum type = GL_FLOAT);

    std::vector<vertex_t> vertices; // might need it for later
    std::vector<unsigned int> indices;

    unsigned int vao, vbo, ebo;
};

#endif // include guard
