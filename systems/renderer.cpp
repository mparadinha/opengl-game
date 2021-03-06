#include <iostream>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glad.h>

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
#include "../components/rigid_body.h"
#include "../components/bounding_volumes.h"
#include "../components/camera.h"
#include "../components/solid_color.h"
#include "../components/animation.h"

Renderer::Renderer(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing renderer..." << std::endl;

    shaders["test_cube"] = new Shader("test_cube");
    shaders["bounding_box"] = new Shader("bounding_box");
    shaders["cubemap"] = new Shader("cubemap");
    shaders["animated"] = new Shader("animated");
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
    case TOGGLE_BONES:
        show_bones = !show_bones;
        break;
    case CLEAN_UP:
        cleanup_meshes();
    }
}

void Renderer::render_all() {
    // skybox needs to render first because we have translucent objects
    render_skybox();

    // fetch camera info component (not going to change during the frame)
    Entity* camera = e_pool.get_special(CAMERA);
    camera_t* camera_info = (camera_t*) camera->components[CAMERA];
    pos_rot_scale_t* prs = (pos_rot_scale_t*) camera->components[POS_ROT_SCALE];
    glm::vec3 camera_pos = prs->pos;

    // need to order meshes because of transparency (closer = rendered last)
    std::vector<Entity*> entities = sorted_entities(camera_pos);

    for(Entity* e : entities) {
        if((e->bitset & ANIMATION) == ANIMATION) {
            animation_t* a = (animation_t*) e->components[ANIMATION];
            shaders["animated"]->use();
            shaders["animated"]->set_uniform("joint_transforms", (float*)a->joint_transforms.data(), a->joint_transforms.size());

            render(shaders["animated"], camera_info, e);
            if(show_bones) {
                auto saved_mesh_ptr = (mesh_t*) e->components[MESH];
                e->components[MESH] = new mesh_t(make_bone_mesh(a));
                glDisable(GL_DEPTH_TEST);
                render(shaders["bounding_box"], camera_info, e);
                glEnable(GL_DEPTH_TEST);
                delete (mesh_t*) e->components[MESH];
                e->components[MESH] = saved_mesh_ptr;
            }
        }
        else {
            render(shaders["test_cube"], camera_info, e);
        }
    }
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

void Renderer::render(Shader* shader, camera_t* camera_info, Entity* e) {
    // send the camera information to the shader
    shader->use();
    shader->set_uniform("view", camera_info->view);
    shader->set_uniform("projection", camera_info->projection);

    // grab the necessary components from the entity
    mesh_t* m = (mesh_t*) e->components[MESH];
    pos_rot_scale_t* prs = (pos_rot_scale_t*) e->components[POS_ROT_SCALE];
    glm::mat4 model = model_matrix(prs);
    shader->set_uniform("model", model);

    // send any adicional information about the mesh to the shader
    if((e->bitset & SOLID_COLOR) == SOLID_COLOR) {
        solid_color_t* color = (solid_color_t*) e->components[SOLID_COLOR];
        shader->set_uniform("color", color->color);
    }

    // tell the gpu to draw the mesh
    glBindVertexArray(m->vao);
    glDrawElements(m->draw_mode, m->num_indices, m->index_data_type, nullptr);
}

void cleanup_meshes() {
    std::vector<Entity*> entities = e_pool.query(MESH);
    for(Entity* e : entities) {
        mesh_t* m = (mesh_t*) e->components[MESH];
        glDeleteVertexArrays(1, &m->vao);
        glDeleteBuffers(MAX_VBOS, m->vbos);
        glDeleteBuffers(1, &m->ebo);
    }
}

std::vector<Entity*> sorted_entities(glm::vec3 camera_pos) {
    std::vector<Entity*> entities = e_pool.query(MESH);
    for(unsigned int i = 1; i < entities.size(); i++) {
        unsigned int j = i;

        rigid_body_t* rb_a = (rigid_body_t*) entities[j - 1]->components[RIGID_BODY];
        rigid_body_t* rb_b = (rigid_body_t*) entities[j]->components[RIGID_BODY];
        float distance_a = glm::length(camera_pos - rb_a->pos);
        float distance_b = glm::length(camera_pos - rb_b->pos);
        while(j > 0 && distance_a < distance_b) {
            std::swap(entities[j], entities[j - 1]);
            j--;
        }
    }

    // move all the imovable objects (floors, walls, etc) to the start
    unsigned int start = 0, i = entities.size() - 1;
    while(i > start) {
        rigid_body_t* rb = (rigid_body_t*) entities[i]->components[RIGID_BODY];
        if(rb->inv_mass == 0) {
            std::swap(entities[start], entities[i]);
            start++;
        }
        else {
            i--;
        }
    }

    return entities;
}

void traverse_bone_tree(std::vector<unsigned int>* indices, std::vector<glm::vec3>* vert_pos, 
        joint_t joint, animation_t* a, glm::vec3 parent_pos) {

    indices->push_back(vert_pos->size());
    vert_pos->push_back(parent_pos);
    indices->push_back(vert_pos->size());
    glm::mat4 tmp = a->joint_transforms[joint.id] * glm::inverse(joint.inverse_bind);
    glm::vec3 pos = glm::vec3(tmp * glm::vec4(0, 0, 0, 1));
    vert_pos->push_back(pos);

    for(auto child : joint.children) traverse_bone_tree(indices, vert_pos, child, a, pos);
}

mesh_t Renderer::make_bone_mesh(animation_t* a) {
    std::vector<unsigned int> indices = {};
    std::vector<glm::vec3> vert_pos = {};
    glm::vec3 root_pos = glm::vec3(a->root_joint.transform * glm::vec4(0, 0, 0, 1));

    traverse_bone_tree(&indices, &vert_pos, a->root_joint, a, root_pos);

    mesh_t mesh;
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao); 

    glGenBuffers(1, &mesh.vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vert_pos.size(), vert_pos.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    mesh.num_indices = indices.size();
    mesh.index_data_type = GL_UNSIGNED_INT;
    mesh.draw_mode = GL_LINES;

    return mesh;
}
