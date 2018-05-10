#ifndef LOADER_H
#define LOADER_H

#include "../message_bus.h"
#include "../components/mesh.h"
#include "../components/texture.h"
#include "../gltf.h"

class Loader : public System {
public:
    Loader(MessageBus* msg_bus);
    ~Loader();

    void handle_message(message_t msg);

    mesh_t load_mesh(std::string filepath);
    texture_t load_texture(std::string path, unsigned int texture_type);

private:
};

void read_buffer_data(gltf::uri_file_t& buffer, gltf::buffer_view_t buf_view, unsigned int data_type);
void load_vbo(gltf::file_t file, gltf::uri_file_t& buffer, unsigned int accessor_idx, unsigned int vbos[], unsigned int vbo_idx, unsigned int size);
void load_image(std::string path, unsigned int texture_type, bool gen_mipmaps = true);

#endif // include guard
