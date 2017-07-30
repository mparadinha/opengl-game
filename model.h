#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

typedef struct {
    Mesh mesh;
    std::vector<texture_t> textures;
} model_node_t;

class Model {
public:
    Model(std::string path);

    void render();

    ~Model() {};

private:
    std::vector<model_node_t> nodes;
    std::string directory;

    unsigned int load_texture(aiString filename);
    model_node_t convert_to_node(aiMesh* mesh, const aiScene* scene);

    void process_node(aiNode* node, const aiScene* scene);
};

#endif // include guard
