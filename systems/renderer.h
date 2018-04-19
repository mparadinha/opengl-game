#ifndef RENDERER_H
#define RENDERER_H

#include "../entity_pool.h"
#include "../message_bus.h"
#include "../shader.h"
#include "../camera.h"
#include "../components/component_enum.h"

class Renderer : public System {
public:
    Renderer(MessageBus* msg_bus);
    ~Renderer();

    void render(Shader& shader, Entity& camera, Entity& e);
    void render(Camera c, Entity e);

private:
    std::map<std::string, Shader*> shaders;
};

#endif // include guard
