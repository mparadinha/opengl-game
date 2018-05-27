#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "renderer.h"
#include "../signals.h"
#include "../entity_pool.h"
#include "../entity_pool_global.h"
#include "../message_bus.h"
#include "../utils/shader.h"

#include "../components/component_enum.h"
#include "../components/mesh.h"
#include "../components/texture.h"
#include "../components/transformation.h"
#include "../components/pos_rot_scale.h"
#include "../components/bounding_volumes.h"
#include "../components/camera.h"

Renderer::Renderer(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing renderer..." << std::endl;

    shaders["test_cube"] = new Shader("test_cube");
    shaders["bounding_box"] = new Shader("bounding_box");
    shaders["cubemap"] = new Shader("cubemap");
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
 
void Renderer::render_all() {
    Shader* shader = shaders["test_cube"];
    for(Entity* e : e_pool.query(MESH)) {
        render(shader, e);
    }

    // draw skybox last so that we don't end up drawing tons of pixels on top of it
    // since most of the skybox wont be visible most of the time
    render_skybox();
}

void Renderer::render_bbs() {
    static Entity* bb_cube = e_pool.get_special(BB_CUBE);
    static mesh_t* bb_mesh = (mesh_t*) bb_cube->components[BB_CUBE];

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // setup shader (we can use the same one for all bounding boxes)
    Entity* camera = e_pool.get_special(CAMERA);
    camera_t* camera_info = (camera_t*) camera->components[CAMERA];
    Shader* shader = shaders["bounding_box"];
    shader->use();
    shader->set_uniform("view", camera_info->view);
    shader->set_uniform("projection", camera_info->projection);

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

void Renderer::render_skybox() {
    // we need a cube mesh for the cubemap (we'll use the same special
    // entity used for drawing bounding boxes)
    static Entity* cube = e_pool.get_special(BB_CUBE);
    static mesh_t* mesh = (mesh_t*) cube->components[BB_CUBE];
    // and the camera
    Entity* camera = e_pool.get_special(CAMERA);
    camera_t* camera_info = (camera_t*) camera->components[CAMERA];
    // and the cubemap textures
    Entity* skybox = e_pool.get_special(SKYBOX);
    texture_t* texture = (texture_t*) skybox->components[SKYBOX];

    // set the needed shader uniforms
    glm::mat4 view = glm::mat4(glm::mat3(camera_info->view)); // ignore translation part
    Shader* shader = shaders["cubemap"];
    shader->use();
    shader->set_uniform("skybox", 0);
    shader->set_uniform("view", view);
    shader->set_uniform("projection", camera_info->projection);

    // do the actual drawing
    // the camera is inside the cube so we need to turn off face culling for this
    // so that opengl doesn't ignore the cube rendering
    // also don't update the zbuffer (depth mask) for the skybox textures
    // as they are always on the background. this prevents unnecessary fragment shaders draws
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    glBindVertexArray(mesh->vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);
    glDrawElements(GL_TRIANGLES, mesh->num_indices, mesh->index_data_type, nullptr);

    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
}

void Renderer::render(Shader* shader, Entity* e) {
    Entity* camera = e_pool.get_special(CAMERA);

    //TODO: check if the entity has at least these components before doing this
    // send the camera information to the shader
    camera_t* camera_info = (camera_t*) camera->components[CAMERA];
    shader->use();
    shader->set_uniform("view", camera_info->view);
    shader->set_uniform("projection", camera_info->projection);

    // grab the necessary components from the entity
    mesh_t* m = (mesh_t*) e->components[MESH];
    pos_rot_scale_t* prs = (pos_rot_scale_t*) e->components[POS_ROT_SCALE];
    glm::mat4 model = model_matrix(prs);
    shader->set_uniform("model", model);

    // tell the gpu to draw the mesh
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->num_indices, m->index_data_type, nullptr);
}
