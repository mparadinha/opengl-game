#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>

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
    void render(Shader* shader, camera_t* camera_info, Entity* e);

private:
    std::map<std::string, Shader*> shaders;

    bool show_bounding_box = false;
};

std::vector<Entity*> sorted_entities(glm::vec3 camera_pos);

#endif // include guard
