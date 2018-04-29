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
#include "input.h"
#include "shader.h"
#include "signals.h"
#include "message_bus.h"
#include "cubemap.h"
#include "basic_geom.h"
#include "axes.h"

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

int main() {
    MessageBus msg_bus = MessageBus();
    Display screen = Display(WINDOW_WIDTH, WINDOW_HEIGHT, "TITLE");

    Loader loader(&msg_bus);
    Renderer renderer(&msg_bus);
    Physics physics(&msg_bus);

    // test entities
    mesh_t test_cube = loader.load_mesh("res/cube.gltf");
    pos_rot_scale_t prs = {{0, 0, 0}, {1, 1, 1}, 0, 0, 0};
    rigid_body_t rb = {{0, 0, 0}};
    aabb_t aabb = {{0, 0, 0}, {1, 1, 1}};
    Entity test_cube_e; test_cube_e.components[POS_ROT_SCALE] = &prs; test_cube_e.components[MESH] = &test_cube; test_cube_e.components[RIGID_BODY] = &rb;
    test_cube_e.components[AABB] = &aabb;
    test_cube_e.bitset = POS_ROT_SCALE | MESH | RIGID_BODY | AABB;

    pos_rot_scale_t prs2 = {{10, 0, 0}, {1, 1, 1}, 0, 0, 0};
    rigid_body_t rb2 = {{10, 0, 0}, {-2, 0, 0}};
    aabb_t aabb2 = {{10, 0, 0}, {1, 1, 1}};
    Entity test_cube_e2; test_cube_e2.components[POS_ROT_SCALE] = &prs2; test_cube_e2.components[MESH] = &test_cube; test_cube_e2.components[RIGID_BODY] = &rb2;
    test_cube_e2.components[AABB] = &aabb2;
    test_cube_e2.bitset = POS_ROT_SCALE | MESH | RIGID_BODY | AABB;

    pos_rot_scale_t c_pos = {glm::vec3(10, 10, 10), glm::vec3(1), 0, 0, 0};
    rigid_body_t rb_c = {{10, 10, 10}, {}, {}, 0, 0, 0, true};
    camera_t c = {glm::mat4(1), glm::perspective(glm::radians(60.0), 1.67, 0.1, 1000.0), 60}; 
    Entity camera_e; camera_e.components[POS_ROT_SCALE] = &c_pos; camera_e.components[CAMERA] = &c; camera_e.components[RIGID_BODY] = &rb_c;
    camera_e.bitset = POS_ROT_SCALE | CAMERA | RIGID_BODY;

    // bounding box cube entity with special component
    Entity bb_cube; bb_cube.components[BB_CUBE] = &test_cube;
    bb_cube.bitset = BB_CUBE;

    e_pool.add_entity(&test_cube_e);
    e_pool.add_entity(&test_cube_e2);
    e_pool.add_entity(&camera_e);
    e_pool.add_entity(&bb_cube);

    renderer.m_camera_info = &c;

    CameraUpdater camera_updater(&msg_bus, &camera_e);

    // shaders
    Shader default_shader("default");
    Shader test_shader("test_cube");
    Shader cubemap_shader("cubemap");
    cubemap_shader.set_uniform("skybox", 0);
    CubeMap skybox("res/skybox/skybox", "jpg");

    // init systems
    Camera camera(&msg_bus, (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT);
    Input input(screen.window, &msg_bus);

    // entities
    Mesh cube = make_cube();
    Mesh sphere = make_sphere(50);

    // testing the axis
    Axes axes;

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
        renderer.render_all(test_shader, camera_e);

        // draw skybox last so that we dont end up drawing tons of pixels on top of it
        // since most of the skybox wont be visible most of the time
        cubemap_shader.use();
        skybox.set_uniforms(cubemap_shader, camera_e);
        skybox.render();

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
