#include <iostream>
#include <fstream>

#include <glad.h>
#include <stb_image.h>

#include "loader.h"
#include "../gltf.h"
#include "../signals.h"
#include "../components/mesh.h"
#include "../components/texture.h"

Loader::Loader(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing loader..." << std::endl;
}

Loader::~Loader() {}

void Loader::handle_message(message_t msg) {
    switch(msg.code) {
    }
}

mesh_t Loader::load_mesh(std::string filepath) {
    //TODO: need to parse and include in the mesh_t the type of data that
    // the indices are. previous bug was because they were sent as single byte
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
    //TODO: unhack this. make a map/table with all the gltf values and corresponding GL types. the hardcoded 5121 is for unsigned bytes
    out_mesh.index_data_type = (indices_accessor.component_type == 5121) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_INT;

    return out_mesh;
}

texture_t Loader::load_texture(std::string path, unsigned int texture_type) {
    texture_t texture; // engine component to be returned

    // allocate space for a texture of type texture_type
    glActiveTexture(GL_TEXTURE0); //TODO: find a way of using multiple textures per shader pass (i.e. having multiple texture units)
    glGenTextures(1, &texture.id);
    glBindTexture(texture_type, texture.id);
    //note: a texture can contain multiple images (e.g. cubemaps have 6)

    // depending on the type of texture the filtering and wrapping option might differ
    // as well as the number of images that need to be loaded
    switch(texture_type) {
    case GL_TEXTURE_2D:
        break;

    case GL_TEXTURE_CUBE_MAP: { // need a new scope because of initing vars here
        // load the six images (one for each cube face)
        std::vector<std::string> ends = {"right", "left", "up", "down", "back", "front"};
        // instead of calling glTexImage2D six times varying the first parameter we
        // can do this insted because the gl enums all continuous (not sure if is this
        // guaranteed from the spec) and in the order of this string vector
        for(unsigned int i = 0; i < ends.size(); i++) {
            std::string img_path = path;
            img_path.insert(img_path.find_last_of("."), "_" + ends[i]);
            load_image(img_path, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, false);
        }

        // set filter and wrapping options
        glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        } // end of scope
        break;

    default:
        std::cerr << "[loader]: texture type " << texture_type << " not supported\n";
        break;
    }

    return texture;
}

void load_vbo(gltf::file_t file, gltf::uri_file_t& buffer, unsigned int accessor_idx, unsigned int vbos[], unsigned int vbo_idx, unsigned int size) {
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

void read_buffer_data(gltf::uri_file_t& buffer, gltf::buffer_view_t buf_view, unsigned int data_type) {
    // put the buffer file pointer pointing to where the data is
    std::vector<unsigned char> data = buffer.read<unsigned char>(
        buf_view.byte_offset, buf_view.byte_length);

    // load data from binary file to GPU
    glBufferData(data_type, /* GL_ARRAY_BUFFER for verts/normals GL_ELEMENT_BUFFER for indices */
        buf_view.byte_length, /* size in bytes */
        &data[0], /* pointer to data */
        GL_STATIC_DRAW);
}

void load_image(std::string path, unsigned int texture_type, bool gen_mipmaps) {
    // load image data from file
    int width, height, num_channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &num_channels, 0);
    if(!data) {
        std::cerr << "[loader]: error loading texture file @ " << path << std::endl;
    }

    // gl format indexed by the number of channels in an image
    unsigned int formats[] = {0, GL_LUMINANCE, 0, GL_RGB, GL_RGBA};

    // send data to GPU
    glTexImage2D(texture_type,
        0, // mipmap level to use this as (if we were manually making the mipmaps)
        formats[num_channels], // *storage* format
        width, height,
        0, // border: must be 0 (legacy stuff)
        formats[num_channels], // format in which we read the image
        GL_UNSIGNED_BYTE, // datatype of the image data (= unsigned char)
        data);

    if(gen_mipmaps) {
        glGenerateMipmap(texture_type);
    }

    stbi_image_free(data);
}
