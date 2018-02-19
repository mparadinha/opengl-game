#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include "mesh.h"
#include "shader.h"

typedef struct {
    Mesh mesh;
    std::vector<texture_t> textures;
} model_node_t;

typedef struct {
    unsigned int vertex_id;
    float weight;
} vertex_weight_t;

typedef struct {
    std::string name;
    std::vector<vertex_weight_t> weights;
    glm::mat4 transform; // relative to this bones parent (aka "bone space")
} bone_t;

class Model {
public:
    Model(std::string path);

    void render();
    void set_bone_uniforms(Shader shader);

    ~Model() {};

private:
    std::vector<model_node_t> nodes;
    std::string directory;
    bool has_animations;
    std::map<std::string, bone_t> bone_map;
    std::vector<bone_t> bones;

    unsigned int load_texture(aiString filename);
    int find_bone(std::string name);
    model_node_t convert_to_node(aiMesh* mesh, const aiScene* scene);

    void process_node(aiNode* node, const aiScene* scene);
};

glm::mat4 ai_to_glm(aiMatrix4x4 m);

#endif // include guard
