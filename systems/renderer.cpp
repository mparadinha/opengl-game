#include <iostream>

#include "renderer.h"
#include "../entity_pool.h"
#include "../components/component_enum.h"
#include "../components/mesh.h"
#include "../components/transformation.h"
#include "../message_bus.h"
#include "../shader.h"

Renderer::Renderer(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing renderer..." << std::endl;

    Shader sun_shader = Shader("onecolor_light");
    sun_shader.set_uniform("light.direction", glm::vec3(-1));
    sun_shader.set_uniform("light.diffuse", glm::vec3(0.7f));
    sun_shader.set_uniform("light.ambient", glm::vec3(0.3f));

    shaders["sun"] = &sun_shader;
}

Renderer::~Renderer() {}

void Renderer::render(Camera c, Entity e) {
    transformation_t* t = (transformation_t*) e.components[TRANSFORMATION];
    mesh_t* m = (mesh_t*) e.components[MESH];

    shaders["sun"]->use();
    shaders["sun"]->set_uniform("diffuse_color", glm::vec3(0.2f));
    c.set_uniforms(*shaders["sun"]);
    //shaders["sun"]->set_uniform("model", get_transform_model(e.components[TRANSFORMATION]))
    shaders["sun"]->set_uniform("model", t->get_model());

    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->num_indices, GL_UNSIGNED_INT, 0);
}
