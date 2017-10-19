#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include <glad.h>

#include "stb_image.h"

#include "model.h"
#include "texture.h"

#include <glm/gtx/string_cast.hpp>

Model::Model(std::string path) {
    // directory of model for loading the images later. the +1 is to include the last '/'
    directory = path.substr(0, path.find_last_of("/") + 1);

    // let assimp do all the grunt work so we get the abstracted scene object
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "[MODEL] - error with assimp import: "
            << importer.GetErrorString() << std::endl;
    }

    // TIP: using this recursive nature might be usefull if we want the
    // parent/child relations between meshes
    // process_node is recursive so calling on the root node takes care of all
    process_node(scene->mRootNode, scene);
}

void Model::render() {
    for(model_node_t node : nodes) {
        for(unsigned int i = 0; i < node.textures.size(); i++) {
            // specify what texture unit we are using (one for each texture)
            // and then just "sync" the shader uniform and bind the gl "object"
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, node.textures[i].id);
        }

        node.mesh.render();
    }
}

void Model::process_node(aiNode* node, const aiScene* scene) {
    // put these node's meshes into the class vector
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        nodes.push_back(convert_to_node(mesh, scene));
    }

    // do same for each of node's children
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

model_node_t Model::convert_to_node(aiMesh* mesh, const aiScene* scene) {
    std::vector<vertex_t> vertices;
    std::vector<unsigned int> indices;
    std::vector<texture_t> textures;

    // put all the vertex data in the array
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertex_t vertex;

        aiVector3D pos = mesh->mVertices[i];
        vertex.position = glm::vec3(pos.x, pos.y, pos.z);

        aiVector3D norm = mesh->mNormals[i];
        vertex.normal = glm::vec3(norm.x, norm.y, norm.z);


        // assimps mesh might have up to 8 texture coords per vertex
        // we only care about the first ones.
        if(mesh->mTextureCoords[0]) {
            aiVector3D tex = mesh->mTextureCoords[0][i];
            vertex.texcoords = glm::vec2(tex.x, tex.y);
        }
        else { // mesh might not have texture coords. we'll fill them in
            vertex.texcoords = glm::vec2(0.0f);
        }

        // finally put the vertex in the array
        vertices.push_back(vertex);
    }

    // now all the indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        for(unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            indices.push_back(mesh->mFaces[i].mIndices[j]);

    // TODO: allow/use more than one texture per texture type
    // each mesh only has one material
    // each material however can have multiple textures of each type
    // but right now we only use one of each and ignore the others
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    texture_t texture; // placeholder to push into vector
    aiString tex_name;

    material->GetTexture(aiTextureType_DIFFUSE, 0, &tex_name);
    texture.type = "diffuse";
    texture.id = load_texture(tex_name);
    textures.push_back(texture);

    material->GetTexture(aiTextureType_SPECULAR, 0, &tex_name);
    texture.type = "specular";
    texture.id = load_texture(tex_name);
    textures.push_back(texture);

    // build the actual mesh object
    model_node_t node = {Mesh(vertices, indices), textures};
    return node;
}

// create and load texture into GPU 
unsigned int Model::load_texture(aiString filename) {
    // fix filename with the path to the folder where models was located
    std::string path = directory + std::string(filename.C_Str());
    Texture tex = Texture(path);
    return tex.load();
}
