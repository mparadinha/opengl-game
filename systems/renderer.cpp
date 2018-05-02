#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "renderer.h"
#include "../signals.h"
#include "../entity_pool.h"
#include "../entity_pool_global.h"
#include "../components/component_enum.h"
#include "../components/mesh.h"
#include "../components/transformation.h"
#include "../components/pos_rot_scale.h"
#include "../components/bounding_volumes.h"
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

    shaders["test_cube"] = new Shader("test_cube");
    shaders["bounding_box"] = new Shader("bounding_box");
}

Renderer::~Renderer() {
    for(const auto& pair : shaders) {
        delete pair.second;
    }
}

void Renderer::handle_message(message_t msg) {
    switch(msg.code) {
    case NEW_FRAME:
        if(show_bounding_box) render_bbs();
        break;
    case TOGGLE_BB:
        show_bounding_box = !show_bounding_box;
        break;
    }
}

void Renderer::render_bbs() {
    static Entity* bb_cube = e_pool.get_special(BB_CUBE);
    static mesh_t* bb_mesh = (mesh_t*) bb_cube->components[BB_CUBE];

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // setup shader (we can use the same one for all bounding boxes)
    Shader* shader = shaders["bounding_box"];
    shader->use();
    shader->set_uniform("view", m_camera_info->view);
    shader->set_uniform("projection", m_camera_info->projection);

    for(auto e : e_pool.query(AABB)) {
        aabb_t* aabb = (aabb_t*) e->components[AABB];
        pos_rot_scale_t prs = {aabb->center, aabb->size};
        glm::mat4 model = model_matrix(&prs);

        shader->set_uniform("model", model);

        glBindVertexArray(bb_mesh->vao);
        glDrawElements(GL_TRIANGLES,
            bb_mesh->num_indices, bb_mesh->index_data_type,
            nullptr);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
    
void Renderer::render_all(Shader& shader) {
    for(auto e : e_pool.query(MESH)) {
        render(shader, *e);
    }
}

void Renderer::render(Shader& shader, Entity& e) {
    Entity* camera = e_pool.get_special(CAMERA);

    //TODO: check if the entity has at least these components before doing this
    // send the camera information to the shader
    camera_t* camera_info = (camera_t*) camera->components[CAMERA];
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
