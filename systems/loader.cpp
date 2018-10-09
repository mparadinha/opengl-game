#include <iostream>
#include <fstream>
#include <map>

#include <glad.h>
#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "loader.h"
#include "../utils/gltf.h"
#include "../signals.h"
#include "../components/mesh.h"
#include "../components/texture.h"
#include "../components/animation.h"

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
    out_mesh.draw_mode = GL_TRIANGLES;

    //TODO: support more than one mesh/primitive/buffer
    gltf::primitive_t primitive = file.meshes[0].primitives[0];
    gltf::uri_file_t buffer(file_dir + "/" + file.buffers[0].uri, file.buffers[0].byte_length);

    // create and bind the vao
    glGenVertexArrays(1, &out_mesh.vao);
    glBindVertexArray(out_mesh.vao);

    // load the mesh attributes each into its own vbo
    std::map<std::string, unsigned int> attrs = primitive.attributes;
    load_vbo(file, buffer, attrs["POSITION"], out_mesh.vbos, 0, 3);
    load_vbo(file, buffer, attrs["NORMAL"], out_mesh.vbos, 1, 3);
    // if there is an animation also load the joint IDS and weights
    if(attrs.count("JOINTS_0") != 0 && attrs.count("WEIGHTS_0") != 0) {
        load_vbo(file, buffer, attrs["JOINTS_0"], out_mesh.vbos, 2, 4);
        load_vbo(file, buffer, attrs["WEIGHTS_0"], out_mesh.vbos, 3, 4);
    }

    // create, bind, and fill ebo
    gltf::accessor_t indices_accessor = file.accessors[primitive.indices];
    glGenBuffers(1, &out_mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_mesh.ebo);
    read_buffer_data(buffer, file.buffer_views[indices_accessor.buffer_view], GL_ELEMENT_ARRAY_BUFFER);
    // and store the number of indices (needed for drawing)
    out_mesh.num_indices = file.accessors[primitive.indices].count;
    out_mesh.index_data_type = gltf::types[indices_accessor.component_type];

    return out_mesh;
}

mesh_t Loader::load_mesh(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices) {
    mesh_t mesh;

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    load_vbo(vertices.data(), vertices.size() * sizeof(glm::vec3), mesh.vbos, 0, 3);

    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    read_buffer_data(indices.data(), indices.size() * sizeof(unsigned int), GL_ELEMENT_ARRAY_BUFFER);

    mesh.num_indices = indices.size();
    mesh.index_data_type = GL_UNSIGNED_INT;

    return mesh;
}

animation_t Loader::load_animation(std::string filepath) {
    std::cout << "loading animation from " << filepath << std::endl;
    std::string file_dir = filepath.substr(0, filepath.find_last_of("/"));
    gltf::file_t file(filepath);

    gltf::uri_file_t buffer(file_dir + "/" + file.buffers[0].uri, file.buffers[0].byte_length);

    animation_t animation;

    // TODO: allow for more than one skin per file

    // load all the information for the joint_t tree
    unsigned int root_node = file.skins[0].joints[0];
    // get all the inverse bind matrices into a map
    gltf::accessor_t accessor = file.accessors[file.skins[0].inverse_bind_matrices];
    std::vector<glm::mat4> inv_binds = buffer.read<glm::mat4>(
        file.buffer_views[accessor.buffer_view].byte_offset, accessor.count);
    // map is: node index -> pair(index into final array, inverse bind matrix)
    std::map<unsigned int, std::pair<unsigned int, glm::mat4>> joint_binds;
    for(unsigned int i = 0; i < inv_binds.size(); i++) {
        joint_binds[file.skins[0].joints[i]] = {i, inv_binds[i]};
    }
    // called for the root node. its recursive and calls for each child
    animation.root_joint = load_joint_tree(file.nodes, root_node, joint_binds);

    // TODO: support more than one animation per file

    float duration = 0;

    // load up all the key frame data
    animation.joint_animations.resize(joint_binds.size());
    for(gltf::channel_t channel : file.animations[0].channels) {
        if(channel.target.path != "translation" && channel.target.path != "rotation") continue;
        // only want channels that refer to a joint
        unsigned int node = channel.target.node;
        if(joint_binds.count(node) == 0) continue;

        gltf::sampler_t sampler = file.animations[0].samplers[channel.sampler];

        // load time
        gltf::accessor_t t_access = file.accessors[sampler.input];
        gltf::buffer_view_t t_buf_v = file.buffer_views[t_access.buffer_view];
        float time = buffer.read<float>(t_buf_v.byte_offset, t_access.count)[0];

        unsigned int joint_anim_index = joint_binds[node].first;
        joint_animation_t& joint_anim = animation.joint_animations[joint_anim_index];
        unsigned int key_frame_index = find_key_frame(joint_anim.key_frames, time);

        joint_anim.key_frames[key_frame_index].time_stamp = time;
        if(time > duration) duration = time;

        // load and save the translation/rotation
        gltf::accessor_t accessor = file.accessors[sampler.output];
        gltf::buffer_view_t buffer_view = file.buffer_views[accessor.buffer_view];
        if(channel.target.path == "translation") {
            glm::vec3 translation = buffer.read<glm::vec3>(buffer_view.byte_offset, 1)[0];
            joint_anim.key_frames[key_frame_index].joint_transform.translation = translation;
        }
        else if(channel.target.path == "rotation") {
            glm::quat rotation = buffer.read<glm::quat>(buffer_view.byte_offset, 1)[0];
            joint_anim.key_frames[key_frame_index].joint_transform.rotation = rotation;
        }
    }

    std::cout << "key frames: " << animation.joint_animations[0].key_frames.size() << std::endl;
    animation.joint_transforms.resize(joint_binds.size());
    for(unsigned int i = 0; i < joint_binds.size(); i++) {
        animation.joint_transforms[i] = glm::mat4(1);
    }

    return animation;
}

unsigned int Loader::find_key_frame(std::vector<key_frame_t>& key_frames, float time) {
    for(unsigned int i = 0; i < key_frames.size(); i++) {
        if(key_frames[i].time_stamp == time) return i;
    }

    key_frames.push_back(key_frame_t());
    return key_frames.size();
}

joint_t Loader::load_joint_tree(std::vector<gltf::node_t> nodes, unsigned int node,
    std::map<unsigned int, std::pair<unsigned int, glm::mat4>> joint_bind_pairs) {

    joint_t joint;
    joint.name = nodes[node].name;
    joint.id = joint_bind_pairs[node].first;
    joint.inverse_bind = joint_bind_pairs[node].second;

    for(unsigned int child_node : nodes[node].children) {
        joint_t child = load_joint_tree(nodes, child_node, joint_bind_pairs);
        joint.children.push_back(child);
    }

    return joint;
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

void create_vbo(unsigned int vbos[], unsigned int vbo_idx, unsigned int size, unsigned int type) { 
    // create and bind vbo
    glGenBuffers(1, &vbos[vbo_idx]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[vbo_idx]);

    // set the attributes for communicating with the shaders 
    glEnableVertexAttribArray(vbo_idx); // in shader: "layout(location = <vbo_idx>)"
    glVertexAttribPointer(vbo_idx,
        size, /* number of components per element of the vbo (e.g. pos=3, texcoord=2) */
        type,
        GL_FALSE, /* don't normalize data */
        0, /* stride */
        (void*) 0); /* pointer to first component of attribute in vbo */
}

void load_vbo(void* data, unsigned int byte_length, unsigned int vbos[], unsigned int vbo_idx, unsigned int size) {
    create_vbo(vbos, vbo_idx, size);

    read_buffer_data(data, byte_length, GL_ARRAY_BUFFER);
}

void load_vbo(gltf::file_t file, gltf::uri_file_t& buffer, unsigned int accessor_idx, unsigned int vbos[], unsigned int vbo_idx, unsigned int size) {
    gltf::accessor_t accessor = file.accessors[accessor_idx];

    create_vbo(vbos, vbo_idx, size, gltf::types[accessor.component_type]);

    gltf::buffer_view_t buf_view = file.buffer_views[accessor.buffer_view];
    read_buffer_data(buffer, buf_view, GL_ARRAY_BUFFER);
}

void read_buffer_data(void* data, unsigned int byte_length, unsigned int data_type) {
    glBufferData(data_type, /* GL_ARRAY_BUFFER for verts/normals GL_ELEMENT_BUFFER for indices */
        byte_length, /* size in bytes */
        data, /* pointer to data */
        GL_STATIC_DRAW);
}

void read_buffer_data(gltf::uri_file_t& buffer, gltf::buffer_view_t buf_view, unsigned int data_type) {
    // put the buffer file pointer pointing to where the data is
    std::vector<unsigned char> data = buffer.read<unsigned char>(
        buf_view.byte_offset, buf_view.byte_length);

    read_buffer_data(&data[0], buf_view.byte_length, data_type);
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
