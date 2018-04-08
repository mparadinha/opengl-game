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

    Shader* sun_shader = new Shader("onecolor_light");
    sun_shader->set_uniform("light.direction", glm::vec3(-1));
    sun_shader->set_uniform("light.diffuse", glm::vec3(0.7f));
    sun_shader->set_uniform("light.ambient", glm::vec3(0.3f));

    shaders["sun"] = sun_shader;

    Shader* test_cube = new Shader("test_cube");
    shaders["test_cube"] = test_cube;
}

Renderer::~Renderer() {
    for(const auto& pair : shaders) {
        delete pair.second;
    }
}

void Renderer::render(Camera c, Entity e) {
    //TODO: check if the entity has at least these components before doing this
    // grab the necessary components from the entity
    transformation_t* t = (transformation_t*) e.components[TRANSFORMATION];
    mesh_t* m = (mesh_t*) e.components[MESH];

    // send the camera information to the shader
    Shader& shader = *shaders["test_cube"];
    shader.use();
    shader.set_uniform("diffuse_color", glm::vec3(0.2f));
    c.set_uniforms(shader);
    shader.set_uniform("model", t->get_model());

    // tell the gpu to draw the mesh
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->num_indices, m->index_data_type, nullptr);
}
