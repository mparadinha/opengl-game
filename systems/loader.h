#ifndef LOADER_H
#define LOADER_H

#include "../message_bus.h"
#include "../components/mesh.h"
#include "../components/animation.h"

class Loader : public System {
public:
    Loader(MessageBus* msg_bus);
    ~Loader();

    mesh_t load_mesh(std::string filepath, bool animated = false);
    animation_t load_animation(std::string filepath);

private:

};

#endif // include guard
