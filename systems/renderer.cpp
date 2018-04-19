#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "renderer.h"
#include "../entity_pool.h"
#include "../components/component_enum.h"
#include "../components/mesh.h"
#include "../components/transformation.h"
#include "../components/pos_rot_scale.h"
#include "../components/camera.h"
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

void Renderer::render(Shader& shader, Entity& camera, Entity& e) {
    //TODO: check if the entity has at least these components before doing this
    // send the camera information to the shader
    camera_t* camera_info = (camera_t*) camera.components[CAMERA];
    //Shader& shader = *shaders["test_cube"];
    shader.use();
    shader.set_uniform("view", camera_info->view);
    shader.set_uniform("projection", camera_info->projection);

    // grab the necessary components from the entity
    mesh_t* m = (mesh_t*) e.components[MESH];
    pos_rot_scale_t* prs = (pos_rot_scale_t*) e.components[POS_ROT_SCALE];
    glm::mat4 model = model_matrix(prs);
    //std::cout << "model matrix: " << glm::to_string(model) << std::endl;
    shader.set_uniform("model", model);

    // tell the gpu to draw the mesh
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->num_indices, m->index_data_type, nullptr);
}
