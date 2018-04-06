#include <iostream>
#include <fstream>

#include <json.hpp>
using json = nlohmann::json;

#include <glad.h>

#include "loader.h"
#include "../gltf.h"
#include "../components/mesh.h"
#include "../components/animation.h"

Loader::Loader(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing loader..." << std::endl;
}

Loader::~Loader() {}

void print(std::ostream& out, const mesh_t& mesh) {
    out << "vao: " << mesh.vao << std::endl;
    out << "vbos[]: ";
    for(int i = 0; i < 2; i++) out << mesh.vbos[i] << " ";
    out << std::endl;
    out << "ebo: " << mesh.ebo << std::endl;
    out << "num_indices: " << mesh.num_indices << std::endl;
}

mesh_t Loader::load_mesh(std::string filepath) {
    //TODO: need to parse and include in the mesh_t the type of data that
    // the indices are. previous was because they were sent as single byte
    // uints and glDrawElements used GL_UNSIGNED_INT instead of GL_UNSIGNED_BYTE
    std::cout << "loading mesh from " << filepath << std::endl;
    std::string file_dir = filepath.substr(0, filepath.find_last_of("/"));
    gltf::file_t file(filepath);

    mesh_t out_mesh; // this is the "engine" mesh that will be used later

    //TODO: support more than one mesh/primitive/buffer
    gltf::primitive_t primitive = file.meshes[0].primitives[0];
    gltf::uri_file_t buffer(file_dir + "/" + file.buffers[0].uri, file.buffers[0].byte_length);

    // create and bind the vao
    glGenVertexArrays(1, &out_mesh.vao);
    glBindVertexArray(out_mesh.vao);

    // load the mesh attributes each into its own vbo
    load_vbo(file, buffer, primitive.attributes["POSITION"], out_mesh.vbos, 0, 3);
    load_vbo(file, buffer, primitive.attributes["NORMAL"], out_mesh.vbos, 1, 3);

    // create, bind, and fill ebo
    gltf::accessor_t indices_accessor = file.accessors[primitive.indices];
    glGenBuffers(1, &out_mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_mesh.ebo);
    read_buffer_data(buffer, file.buffer_views[indices_accessor.buffer_view], GL_ELEMENT_ARRAY_BUFFER);
    // and store the number of indices (needed for drawing)
    out_mesh.num_indices = file.accessors[primitive.indices].count;
    //TODO: unhack this. make a map/table with all the gltf values and corresponding GL types. the hardcoded 5120 is for unsigned bytes
    out_mesh.index_data_type = (indices_accessor.component_type == 5120) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_INT;

    std::cout << "loaded mesh info:\n";
    print(std::cout, out_mesh);

    return out_mesh;
}

void read_buffer_data(gltf::uri_file_t& buffer, gltf::buffer_view_t buf_view, unsigned int data_type) {
    // put the buffer file pointer pointing to where the data is
    std::vector<unsigned char> data = buffer.read<unsigned char>(
        buf_view.byte_offset, buf_view.byte_length);

    std::cout << "buffer_view_t:\n";
    std::cout << "    - buffer: " << buf_view.buffer << std::endl;
    std::cout << "    - byte_offset: " << buf_view.byte_offset << std::endl;
    std::cout << "    - byte_length: " << buf_view.byte_length << std::endl;
    std::cout << "    - byte_stride: " << buf_view.byte_stride << std::endl;

    // load data from binary file to GPU
    glBufferData(data_type, /* GL_ARRAY_BUFFER for verts/normals GL_ELEMENT_BUFFER for indices */
        buf_view.byte_length, /* size in bytes */
        &data[0], /* pointer to data */
        GL_STATIC_DRAW);

    std::cout << "glBufferData(" << data_type << ", " << buf_view.byte_length << ", "
        << (void*) &data[0]<< ", " << "GL_STATIC_DRAW)" << std::endl;
}

void load_vbo(gltf::file_t file, gltf::uri_file_t& buffer, unsigned int accessor_idx, unsigned int vbos[], unsigned int vbo_idx, unsigned int size) {
    std::cout << "load_vbo(file, buffer, " << accessor_idx << ", " << vbos << ", " << vbo_idx << ", " << size << ")" << std::endl;

    // create and bind vbo
    glGenBuffers(1, &vbos[vbo_idx]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[vbo_idx]);

    gltf::accessor_t accessor = file.accessors[accessor_idx];
    gltf::buffer_view_t buf_view = file.buffer_views[accessor.buffer_view];

    read_buffer_data(buffer, buf_view, GL_ARRAY_BUFFER);

    // set the attributes for communicating with the shaders 
    glEnableVertexAttribArray(vbo_idx); // in shader: "layout(location = <vbo_idx>)"
    glVertexAttribPointer(vbo_idx,
        size, /* number of components per element of the vbo (e.g. pos=3, texcoord=2) */
        GL_FLOAT,
        GL_FALSE, /* don't normalize data */
        0, /* stride */
        (void*) 0); /* pointer to first component of attribute in vbo */

    std::cout << std::endl;
}

/* eww dont look pls
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
}*/
