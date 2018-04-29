#ifndef RENDERER_H
#define RENDERER_H

#include "../entity_pool.h"
#include "../message_bus.h"
#include "../shader.h"
#include "../camera.h"
#include "../components/component_enum.h"
#include "../components/camera.h"

class Renderer : public System {
public:
    Renderer(MessageBus* msg_bus);
    ~Renderer();

    void handle_message(message_t msg);

    void render_bbs();
    void render_all(Shader& shader, Entity& camera);
    void render(Shader& shader, Entity& camera, Entity& e);
    void render(Camera c, Entity e);

    camera_t* m_camera_info;

private:
    std::map<std::string, Shader*> shaders;

    bool show_bounding_box = false;
};

#endif // include guard
