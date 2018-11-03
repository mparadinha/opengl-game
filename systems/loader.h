#ifndef LOADER_H
#define LOADER_H

#include <vector>

#include "../message_bus.h"
#include "../components/mesh.h"
#include "../components/texture.h"
#include "../components/animation.h"
#include "../utils/gltf.h"

struct vertex_t {
    glm::vec2 uv;
    glm::vec3 pos, norm;
    glm::vec4 color;
};

class Loader : public System {
public:
    Loader(MessageBus* msg_bus);
    ~Loader();

    void handle_message(message_t msg);

    mesh_t load_mesh(std::string filepath);
    mesh_t load_mesh(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices);
    animation_t load_animation(std::string filepath);
    texture_t load_texture(std::string path, unsigned int texture_type);

private:
    joint_t load_joint_tree(std::vector<gltf::node_t> nodes, unsigned int node,
        std::map<unsigned int, std::pair<unsigned int, glm::mat4>> joint_bind_pairs);
    unsigned int find_key_frame(std::vector<key_frame_t>& key_frames, float time);
};

void read_buffer_data(void* data, unsigned int byte_length, unsigned int data_type);
void read_buffer_data(gltf::uri_file_t& buffer, gltf::accessor_t& accessor, gltf::buffer_view_t buf_view, unsigned int data_type);
void load_vbo(gltf::file_t file, gltf::uri_file_t& buffer, unsigned int accessor_idx, unsigned int vbos[], unsigned int vbo_idx, unsigned int size);
void load_vbo(void* data, unsigned int byte_length, unsigned int vbos[], unsigned int vbo_idx, unsigned int size, unsigned int type = GL_FLOAT);
void create_vbo(unsigned int vbos[], unsigned int vbo_idx, unsigned int size, unsigned int type);
void load_image(std::string path, unsigned int texture_type, bool gen_mipmaps = true);

#endif // include guard
