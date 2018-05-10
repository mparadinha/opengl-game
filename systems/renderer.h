#ifndef RENDERER_H
#define RENDERER_H

#include "../entity_pool.h"
#include "../message_bus.h"
#include "../utils/shader.h"
#include "../components/component_enum.h"
#include "../components/camera.h"

class Renderer : public System {
public:
    Renderer(MessageBus* msg_bus);
    ~Renderer();

    void handle_message(message_t msg);

    void render_bbs();
    void render_skybox();
    void render_all();
    void render(Shader* shader, Entity* e);

private:
    std::map<std::string, Shader*> shaders;

    bool show_bounding_box = true;
};

#endif // include guard
