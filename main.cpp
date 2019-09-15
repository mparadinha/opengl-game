#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "display.h"
#include "signals.h"
#include "message_bus.h"

#include "utils/shader.h"
#include "utils/axes.h"
#include "utils/gltf.h"

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
#include "components/solid_color.h"
#include "components/animation.h"

#include "entity_pool_global.h"

static const unsigned int WINDOW_WIDTH = 1600;
static const unsigned int WINDOW_HEIGHT = 900;

void display_fps(float dt);
void wait(float seconds);

unsigned int add_cube(Loader& loader, glm::vec3 pos, glm::vec3 scale = {1, 1, 1}, glm::vec3 vel = {0, 0, 0}, bool floating = false, glm::vec4 color = {1, 1, 1, 1}) {
    static mesh_t* mesh = new mesh_t(loader.load_mesh("res/cube.gltf"));
    pos_rot_scale_t* prs = new pos_rot_scale_t({pos, scale});

    const float density = 1;
    float inv_mass = floating ? 0 : 1 / (density * scale.x * scale.y * scale.z * 8);
    rigid_body_t* rb = new rigid_body_t({
        pos, vel, scale,
        0, 0, 0, // angles
        inv_mass, floating,
        {0.25, 0.4} // material properties
    });
    aabb_t* aabb = new aabb_t({pos, scale});

    solid_color_t* solid_color = new solid_color_t({color});

    Entity* cube = new Entity;
    cube->components[MESH] = mesh;
    cube->components[POS_ROT_SCALE] = prs;
    cube->components[RIGID_BODY] = rb;
    cube->components[AABB] = aabb;
    cube->components[SOLID_COLOR] = solid_color;
    cube->bitset = MESH | POS_ROT_SCALE | RIGID_BODY | AABB | SOLID_COLOR;

    return e_pool.add_entity(cube);
}

unsigned int add_grid(Loader& loader, glm::vec3 pos, glm::vec3 scale, unsigned int divs, float size) {
    Entity* test = new Entity;
    std::vector<glm::vec3> vertices;
    for(uint32_t i = 0; i < divs + 1; i++) {
        vertices.push_back({size * i / float(divs), 0, 0});
        vertices.push_back({size * i / float(divs), 0, size});
        vertices.push_back({0, 0, size * i / divs});
        vertices.push_back({size, 0, size * i / divs});
    }
    //for(auto v : vertices) std::cout << glm::to_string(v) << std::endl;
    std::vector<unsigned int> indices;
    for(uint32_t i = 0; i < (divs + 1) * 4; i++) {
        indices.push_back(i);
    }
    //for(auto i : indices) std::cout << i << std::endl; 
    mesh_t* testmesh = new mesh_t(loader.load_mesh(vertices, indices));
    testmesh->draw_mode = GL_LINES;
    test->components[MESH] = testmesh;
    test->components[POS_ROT_SCALE] = new pos_rot_scale_t({pos, scale, 0, 0, 0});
    test->components[RIGID_BODY] = new rigid_body_t({pos, {0, 0, 0}, scale, 0, 0, 0, 0, true});
    test->components[SOLID_COLOR] = new solid_color_t({{1, 1, 1, 1}});
    test->bitset = MESH | POS_ROT_SCALE | RIGID_BODY | SOLID_COLOR;
    return e_pool.add_entity(test);
}

unsigned int add_animated(Loader& loader, std::string path, glm::vec3 pos, glm::vec3 scale, glm::vec3 angles) {
    Entity* thot = new Entity;
    mesh_t* thotm = new mesh_t(loader.load_mesh(path));
    animation_t* thota = new animation_t(loader.load_animation(path));
    thot->components[MESH] = thotm;
    thot->components[ANIMATION] = thota;
    thot->components[POS_ROT_SCALE] = new pos_rot_scale_t({pos, scale, angles.x, angles.y, angles.z});
    thot->components[RIGID_BODY] = new rigid_body_t({pos, {0, 0, 0}, scale, angles.x, angles.y, angles.z, 0, true});
    thot->components[SOLID_COLOR] = new solid_color_t({{1, 1, 1, 1}});
    thot->bitset = MESH | POS_ROT_SCALE | RIGID_BODY | SOLID_COLOR | ANIMATION;
    return e_pool.add_entity(thot);
}

int main() {
    MessageBus msg_bus = MessageBus();

    Display screen = Display(WINDOW_WIDTH, WINDOW_HEIGHT, "i3floatme");

    // init systems
    Input input(screen.window, &msg_bus);
    Loader loader(&msg_bus);
    Renderer renderer(&msg_bus);
    Physics physics(&msg_bus);
    CameraUpdater camera_updater(&msg_bus);
    Animator animator(&msg_bus);

    // test entities
    //add_cube(loader, {0, 5, 0}, {1, 1.5, 1}, {0, 0, 0}, false, {0, 0, 1, 0.75});
    //add_cube(loader, {10, 0, 0}, {1, 1, 1}, {-5, 0, 0}, false, {0, 1, 0, 1});
    //add_cube(loader, {5, 20.5, 5}, {1, 2.5, 1}, {0, 0, 0}, false, {1, 0, 0, 0.4});
    add_cube(loader, {0, -2, 0}, {100, 0.1, 100}, {0, 0, 0}, true, {0.2, 0.2, 0.2, 0.9});
    add_cube(loader, {20, 10, 20}, {10, 10, 10}, {0, 0, 0}, true, {0.2, 0.2, 0.2, 0.9});
    add_grid(loader, {0, -1.89, 0}, {1, 1, 1}, 10, 10);

    glm::vec3 p(0, 0, 0), s(1);
    float thot_angle = -90;
    add_animated(loader, "/home/parada/repos/opengl-game/res/glTF-Sample-Models-master/2.0/CesiumMan/glTF/CesiumMan.gltf", p, s, {0, thot_angle, 0});
    add_animated(loader, "/home/parada/repos/opengl-game/res/glTF-Sample-Models-master/2.0/Monster/glTF/Monster.gltf", p, glm::vec3(0.005), {0, thot_angle, 0});
    add_animated(loader, "/home/parada/repos/opengl-game/res/thot_dab.gltf", {0, 0, 4}, s, {0, 0, 0});

    // special entities
    // add a simple cube mesh to the pool to draw bounding boxes later
    mesh_t* test_cube = new mesh_t(loader.load_mesh("res/cube.gltf"));
    Entity* bb_cube = new Entity;
    bb_cube->components[BB_CUBE] = test_cube;
    bb_cube->bitset = BB_CUBE;
    e_pool.add_special(bb_cube, BB_CUBE);
    // add skybox texture to special pool
    texture_t skybox_texture = loader.load_texture("res/skybox/skybox.jpg", GL_TEXTURE_CUBE_MAP);
    Entity _skybox; _skybox.components[SKYBOX] = &skybox_texture;
    _skybox.bitset = SKYBOX;
    e_pool.add_special(&_skybox, SKYBOX);

    float dt, time, last_time = glfwGetTime();
    message_t new_frame_msg = {NEW_FRAME, {0}};
    bool show_fps = false;
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

    msg_bus.add_message({CLEAN_UP}, PRIORITY_NOW);

    return 0;
}

void display_fps(float dt) {
    static int frames = 0;
    static float time = 0;
    if(time >= 1) {
        std::cout << "FPS: " << frames / time << std::endl;
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
