#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "display.h"
#include "signals.h"
#include "message_bus.h"

#include "utils/shader.h"
#include "utils/axes.h"

#include "systems/input.h"
#include "systems/renderer.h"
#include "systems/loader.h"
#include "systems/animator.h"
#include "systems/camera.h"
#include "systems/physics.h"

#include "components/component_enum.h"
#include "components/transformation.h"
#include "components/pos_rot_scale.h"
#include "components/mesh.h"
#include "components/camera.h"
#include "components/rigid_body.h"

#include "entity_pool_global.h"

static const unsigned int WINDOW_WIDTH = 1600;
static const unsigned int WINDOW_HEIGHT = 900;

void display_fps(float dt);
void wait(float seconds);

void add_cube(Loader& loader, glm::vec3 pos, glm::vec3 scale = {1, 1, 1}, glm::vec3 vel = {0, 0, 0}) {
    static mesh_t* mesh = new mesh_t(loader.load_mesh("res/cube.gltf"));
    pos_rot_scale_t* prs = new pos_rot_scale_t({pos, scale});
    rigid_body_t* rb = new rigid_body_t({pos, vel, scale, 0, 0, 0, false});
    aabb_t* aabb = new aabb_t({pos, scale});

    Entity* cube = new Entity;
    cube->components[MESH] = mesh;
    cube->components[POS_ROT_SCALE] = prs;
    cube->components[RIGID_BODY] = rb;
    cube->components[AABB] = aabb;
    cube->bitset = MESH | POS_ROT_SCALE | RIGID_BODY | AABB;

    e_pool.add_entity(cube);
}

int main() {
    MessageBus msg_bus = MessageBus();

    Display screen = Display(WINDOW_WIDTH, WINDOW_HEIGHT, "TITLE");

    // init systems
    Input input(screen.window, &msg_bus);
    Loader loader(&msg_bus);
    Renderer renderer(&msg_bus);
    Physics physics(&msg_bus);
    CameraUpdater camera_updater(&msg_bus);

    // test entities
    add_cube(loader, {0, 0, 0});
    add_cube(loader, {10, 0, 0}, {1, 1, 1}, {-2, 0, 0});

    // special entities
    // add a simple cube mesh to the pool to draw bounding boxes later
    mesh_t test_cube = loader.load_mesh("res/cube.gltf");
    Entity bb_cube;
    bb_cube.components[BB_CUBE] = &test_cube;
    bb_cube.bitset = BB_CUBE;
    e_pool.add_special(&bb_cube, BB_CUBE);
    // add skybox texture to special pool
    texture_t skybox_texture = loader.load_texture("res/skybox/skybox.jpg", GL_TEXTURE_CUBE_MAP);
    Entity _skybox; _skybox.components[SKYBOX] = &skybox_texture;
    _skybox.bitset = SKYBOX;
    e_pool.add_special(&_skybox, SKYBOX);

    float dt, time, last_time = glfwGetTime();
    message_t new_frame_msg = {NEW_FRAME, {0}};
    bool show_fps = true;
    unsigned int max_fps = 60;
    float spf = 1 / (float) max_fps;
    while(!input.should_close()) {
        // calculate delta time (in seconds) for this frame
        time = glfwGetTime();
        dt = time - last_time;
        last_time = time;

        // TODO: make this appear as text on screen
        if(show_fps) display_fps(dt);

        screen.clear();

        // post message to all systems that a new frame has started
        new_frame_msg.data[0] = dt;
        msg_bus.add_message(new_frame_msg, PRIORITY_NOW);
        
        // draw entities
        renderer.render_all();

        msg_bus.process();

        screen.swap_buffers();

        // max frame rate. if there is still time in this frame wait
        if(dt < spf) wait(spf - dt);
    }

    return 0;
}

void display_fps(float dt) {
    static int frames = 0;
    static float time = 0;
    if(time >= 1) {
        //std::cout << "FPS: " << frames / time << std::endl;
        frames = 0;
        time = 0;
    }
    else {
        frames++;
        time += dt;
    }
}

void wait(float seconds) {
    auto duration = std::chrono::microseconds((unsigned int) seconds * 1000000);
    //std::cout << "waiting " << seconds * 1000 << "ms\n";
    std::this_thread::sleep_for(duration);
}
