#ifndef GLTF_H
#define GLTF_H

#include <map>
#include <vector>
#include <string>

#include <glad.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace gltf {

// this is not suppost to ever change at runtime but the compiler freaks out
// because of accessing it cannot be done while mainting const certain
static std::map<unsigned int, unsigned int> types = { 
    {5121, GL_UNSIGNED_BYTE},
    {5123, GL_UNSIGNED_SHORT},
    {5125, GL_UNSIGNED_INT},
    {5126, GL_FLOAT}
};

struct buffer_t {
    buffer_t(std::map<std::string, std::string>& in);

    unsigned int byte_length;
    std::string uri;
};

struct buffer_view_t {
    buffer_view_t(std::map<std::string, std::string>& in);

    unsigned int buffer;
    unsigned int byte_offset, byte_length, byte_stride;
    unsigned int target;
};

struct accessor_t {
    accessor_t(std::map<std::string, std::string>& in);

    unsigned int buffer_view;
    unsigned int byte_offset;
    std::string type;
    unsigned int component_type;
    unsigned int count;
    std::vector<float> min, max;
};

struct primitive_t {
    primitive_t(std::map<std::string, std::string>& in);

    unsigned int mode, indices, material;
    std::map<std::string, unsigned int> attributes;
};

struct mesh_t {
    mesh_t(std::map<std::string, std::string>& in);

    std::string name;
    std::vector<primitive_t> primitives;
};

struct skin_t {
    skin_t(std::map<std::string, std::string>& in);

    unsigned int inverse_bind_matrices; // accessor that has 1 matrix per joint
    std::vector<unsigned int> joints;
    unsigned int skeleton; // points to node that is root of joint hierarchy
};

struct node_t {
    node_t(std::map<std::string, std::string>& in);

    std::vector<unsigned int> children; // indices on list of nodes
    // the node can have a transformation either in the form of a single mat4
    // or separate translation and scale vec3's and a quaterion rotation which
    // converted to a matrix on initialization. M = T * R * S
    // attention
    glm::mat4 matrix;
    // or 3 separate properties for trans, rot and scale
    glm::vec3 translation, scale;
    glm::quat rotation;
    unsigned int mesh, camera, skin;
    std::string name; // optional
};

struct target_t {
    target_t(std::map<std::string, std::string>& in);
    target_t() {};

    unsigned int node;
    std::string path; // name of the animated property. can be:
    // "translation", "rotation" and "scale" (affects the local transform of the node)
    // "weights"
};

struct channel_t {
    channel_t(std::map<std::string, std::string>& in);

    target_t target;
    unsigned int sampler; // summarizes the actual animation data
};

struct sampler_t {
    sampler_t(std::map<std::string, std::string>& in);

    unsigned int input, output; // indices for accessors
    // "input" has time of the key frame, "output" has values for animated property
    std::string interpolation; // can be: "LINEAR", "STEP", "CATMULLROMSPLINE", "CUBICSPLINE"
};

struct animation_t {
    animation_t(std::map<std::string, std::string>& in);

    std::vector<channel_t> channels;
    std::vector<sampler_t> samplers;
};

struct asset_t {
    asset_t(std::map<std::string, std::string>& pairs);
    asset_t() {};

    std::string generator, version;    
};

struct pbr_metallic_t {
    pbr_metallic_t(std::map<std::string, std::string>& in);
    pbr_metallic_t() {};

    std::vector<float> base_color_factor;
    float metallic_factor;
};
 
struct material_t {
    material_t(std::map<std::string, std::string>& in);

    std::string name;
    pbr_metallic_t pbr_metallic_roughness;
};

struct scene_t { 
    scene_t(std::map<std::string, std::string>& in);

    std::string name;
    std::vector<unsigned int> nodes;
};

struct file_t {
    file_t(const std::string& filepath);

    std::vector<accessor_t> accessors;
    std::vector<buffer_view_t> buffer_views;
    std::vector<buffer_t> buffers;
    std::vector<skin_t> skins;
    std::vector<animation_t> animations;
    std::vector<node_t> nodes;
    std::vector<material_t> materials;
    std::vector<mesh_t> meshes;
    asset_t asset;
    unsigned int scene;
    std::vector<scene_t> scenes;
};

struct uri_file_t {
    uri_file_t(std::string filepath, unsigned int bytes);
    ~uri_file_t();

    FILE* file;

    void seek(unsigned int offset);

    template<typename T = unsigned char> // default to reading bytes
    std::vector<T> read(unsigned int offset, unsigned int count) {
        std::cout << "read(" << offset << ", " << count << ")" << std::endl;
    
        seek(offset); // set file pointer to the offset position
    
        T* data = new T[count];
        if(!data) {
            std::cout << "ERROR: couldn't create array for " << count * sizeof(T)
                << " bytes" << std::endl;
        }
    
        fread(data, sizeof(T), count, file);
    
        // convert c style array to c++ std::vector
        std::vector<T> vec(data, data + count);
    
        delete data;
    
        return vec;
    }
};

} // end of gltf namespace

#endif // include guard
