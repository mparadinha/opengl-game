#ifndef GLTF_H
#define GLTF_H

#include <map>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace gltf {

struct buffer_t {
    buffer_t(std::ifstream& in);

    unsigned int byte_length;
    std::string uri;
};

struct buffer_view_t {
    buffer_view_t(std::ifstream& in);

    unsigned int buffer;
    unsigned int byte_offset, byte_length, byte_stride;
    unsigned int target;
};

struct accessor_t {
    accessor_t(std::ifstream& in);

    unsigned int buffer_view;
    unsigned int byte_offset;
    std::string type;
    unsigned int component_type;
    unsigned int count;
    std::vector<float> min, max;
};

struct primitive_t {
    primitive_t(std::ifstream& in);

    unsigned int mode, indices, material;
    std::map<std::string, unsigned int> attributes;
};

struct mesh_t {
    mesh_t(std::ifstream& in);

    std::string name;
    std::vector<primitive_t> primitives;
};

struct meshes_t {
    void init(std::ifstream& in);

    std::vector<mesh_t> meshes;
    std::vector<float> weights; // of (possible) morph targets
};

struct skin_t {
    skin_t(std::ifstream& in);

    unsigned int inverse_bind_matrix; // accessor that has 1 matrix per joint
    std::vector<unsigned int> joints;
    unsigned int skeleton; // points to node that is root of joint hierarchy
};

struct node_t {
    node_t(std::ifstream& in);

    std::vector<unsigned int> children; // indices on list of nodes
    glm::mat4 matrix; // can have a single matrix; M = T * R * S
    // or 3 separate properties for trans, rot and scale
    unsigned int mesh, camera, skin;
    std::string name; // optional
};

struct target_t {
    target_t(std::ifstream& in);
    target_t() {};

    unsigned int node;
    std::string path; // name of the animated property. can be:
    // "translation", "rotation" and "scale" (affects the local transform of the node)
    // "weights"
};

struct channel_t {
    channel_t(std::ifstream& in);

    target_t target;
    unsigned int sampler; // summarizes the actual animation data
};

struct sampler_t {
    sampler_t(std::ifstream& in);

    unsigned int input, output; // indices for accessors
    // "input" has time of the key frame, "output" has values for animated property
    std::string interpolation; // can be: "LINEAR", "STEP", "CATMULLROMSPLINE", "CUBICSPLINE"
};

struct animation_t {
    animation_t(std::ifstream& in);

    std::vector<channel_t> channels;
    std::vector<sampler_t> samplers;
};

struct asset_t {
    void init(std::ifstream& in);

    std::string generator, version;    
};

struct pbr_metallic_t {
    void init(std::ifstream& in);

    std::vector<float> base_color_factor;
    float metallic_factor;
};
 
struct material_t {
    material_t(std::ifstream& in);

    std::string name;
    pbr_metallic_t pbr_metallic_roughness;
};

struct scene_t { 
    scene_t(std::ifstream& in);

    std::string name;
    std::vector<unsigned int> nodes;
};

struct file_t {
    file_t(std::string filepath);

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
    template<typename T>
    std::vector<T> read(unsigned int offset, unsigned int count);
};

} // end of gltf namespace

/* convenience functions */
float to_f(std::string);
unsigned int to_uint(std::string);
unsigned int read_uint(std::ifstream& in);
std::string read_string(std::ifstream& in);
std::vector<float> read_float_vector(std::string source);
std::vector<unsigned int> read_uint_vector(std::string source);
glm::vec3 read_glm_vector(std::string source);
glm::quat read_glm_quat(std::string source);
glm::mat4 read_glm_matrix(std::string source);
std::vector<std::string> split_vector_str(std::string source);
std::vector<std::string> split_str(std::string source, char separator);
std::vector<std::string> split_str_last_of(std::string source, char separator);
std::map<std::string, std::string> get_pairs(std::ifstream& in);
std::map<std::string, std::string> get_basic_pairs(std::string content);
std::vector<std::string> read_pair(std::ifstream& in);
void clean_str(std::string& s);

std::string read_obj(std::ifstream& in);
std::vector<std::string> read_obj_list(std::ifstream& in);

#endif // include guard
