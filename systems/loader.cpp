#include <iostream>
#include <fstream>

#include <json.hpp>
using json = nlohmann::json;

#include <glad.h>

#include "loader.h"
#include "../components/mesh.h"
#include "../components/animation.h"

Loader::Loader(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing loader..." << std::endl;
}

Loader::~Loader() {}

mesh_t Loader::load_mesh(std::string filepath, bool animated) {
    // open gltf json file
    std::ifstream file(filepath);
    json gltf = json::parse(file);

    // fetch some objects from the json now for convinience
    auto accessors = gltf["accessors"];
    auto meshes = gltf["meshes"]; auto mesh0 = meshes[0];
    //std::cout << "mesh0: " << mesh0 << std::endl;
    auto primitives = mesh0["primitives"];
    auto primitive = primitives[0];
    auto attributes = primitive["attributes"];
    auto buffer_views = gltf["bufferViews"];

    int pos_attrib = (int) attributes["POSITION"];
    int n_attrib = (int) attributes["NORMAL"];
    auto position_access = accessors[pos_attrib];
    auto normal_access = accessors[n_attrib];
    int position_buffer = position_access["bufferView"];
    int normal_buffer = normal_access["bufferView"];
    auto positions_info = buffer_views[position_buffer];
    auto normals_info = buffer_views[normal_buffer];
    std::cout << "normals_info: " << normals_info << std::endl;
    int byte_length = (int) positions_info["byteLength"] + (int) normals_info["byteLength"];

    // the vertex data is in a binary file. we need to open that as well
    auto buffers = gltf["buffers"];
    auto bufferino = buffers[0];
    auto binary_file = bufferino["uri"];
    std::ifstream binary(binary_file, std::ios::binary);
    std::vector<float> binary_data;
    for(int i = 0; i < byte_length; i++) {
        float f;
        binary >> f;
        binary_data.push_back(f);
    }

    unsigned int vao;
    glGenVertexArrays(1, &vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (int) positions_info["byteLength"] + (int) normals_info["byteLength"], &binary_data[0], GL_STATIC_DRAW);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    // set the attributes
    int size = positions_info["byteOffset"];
    size_t stride = (size_t) size;
    glEnableVertexAttribArray(0); // positions
    glVertexAttribPointer(0, (int) positions_info["byteLength"], GL_FLOAT, GL_FALSE, sizeof(GLfloat),
        (void *) stride);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    int n_off = normals_info["byteOffset"];
    size_t n_stride = (size_t) n_off;
    int n_len = normals_info["byteLength"];
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    glEnableVertexAttribArray(1); // normals
    glVertexAttribPointer(1, n_len, GL_FLOAT, GL_FALSE, sizeof(GLfloat),
        (void *) n_stride);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    auto indices_info = buffer_views[(int) ((accessors[(int) (primitive["indices"])])["bufferView"])]; 
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << indices_info << std::endl;
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        (int) indices_info["byteLength"],
        &binary_data[0],
        GL_STATIC_DRAW
    );
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    unsigned int num_indices = ((int) indices_info["byteLength"]) / ((int) sizeof(unsigned int));
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    mesh_t mesh;
    mesh.vao = vao;
    mesh.vbo = vbo;
    mesh.ebo = ebo;
    mesh.num_indices = num_indices;
    return mesh;
}

animation_t Loader::load_animation(std::string filepath) {
    // load the json file into the json object
    std::ifstream file(filepath);
    json gltf = json::parse(file);
    file.close();

    animation_t anim;
    std::vector<key_frame_t> frames;
    anim.key_frames = frames;
    anim.duration = 0;
    return anim;
}
