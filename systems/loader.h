#ifndef LOADER_H
#define LOADER_H

#include "../message_bus.h"
#include "../components/mesh.h"
#include "../components/animation.h"
#include "../gltf.h"

#include "../mesh.h"

class Loader : public System {
public:
    Loader(MessageBus* msg_bus);
    ~Loader();

    mesh_t load_mesh(std::string filepath);
    float * read_uri(std::string filepath);

    /*mesh_t load_mesh(std::string filepath, bool animated = false); 
    animation_t load_animation(std::string filepath);*/

private:

};

void read_buffer_data(gltf::uri_file_t& buffer, gltf::buffer_view_t buf_view, unsigned int data_type);
void load_vbo(gltf::file_t file, gltf::uri_file_t& buffer, unsigned int accessor_idx, unsigned int vbos[], unsigned int vbo_idx, unsigned int size);

#endif // include guard
