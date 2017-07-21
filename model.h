#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

class Model {
public:
    Model(std::string path);

    void render();

    ~Model() {};

private:
    std::vector<Mesh> meshes;
    std::vector<texture_t> textures;
    std::string directory;

    unsigned int load_texture(aiString filename);
    Mesh convert_to_mesh(aiMesh* mesh, const aiScene* scene);

    void process_node(aiNode* node, const aiScene* scene);
};

#endif // include guard
