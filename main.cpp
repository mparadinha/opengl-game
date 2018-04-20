#include <iostream>
#include <ctime>

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "display.h"
#include "input.h"
#include "shader.h"
#include "game_object.h"
#include "fps_camera.h"
#include "model.h"
#include "signals.h"
#include "message_bus.h"
#include "cubemap.h"
#include "terrain.h"
#include "quad.h"
#include "player.h"
#include "basic_geom.h"
#include "gltf.h"
#include "axes.h"

#include "systems/renderer.h"
#include "systems/loader.h"
#include "systems/animator.h"
#include "systems/camera.h"

#include "components/component_enum.h"
#include "components/transformation.h"
#include "components/pos_rot_scale.h"
#include "components/mesh.h"
#include "components/camera.h"

static const unsigned int WINDOW_WIDTH = 1600;
static const unsigned int WINDOW_HEIGHT = 900;

void display_fps(float dt);

int main() {
    MessageBus msg_bus = MessageBus();
    Display screen = Display(WINDOW_WIDTH, WINDOW_HEIGHT, "TITLE");

    Renderer renderer(&msg_bus);
    Loader loader(&msg_bus);

    // test entities
    mesh_t test_cube = loader.load_mesh("res/cube.gltf");
    pos_rot_scale_t prs = {{0, 0, 0}, {1, 1, 1}, 0, 0, 0};
    Entity test_cube_e; test_cube_e.components[POS_ROT_SCALE] = &prs; test_cube_e.components[MESH] = &test_cube;

    pos_rot_scale_t c_pos = {glm::vec3(10, 10, 10), glm::vec3(1), 0, 0, 0};
    camera_t c = {glm::mat4(1), glm::perspective(glm::radians(60.0), 1.67, 0.1, 1000.0), 60}; 
    Entity camera_e; camera_e.components[POS_ROT_SCALE] = &c_pos; camera_e.components[CAMERA] = &c;
 
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
        renderer.render(test_shader, camera_e, test_cube_e);

        // draw skybox last so that we dont end up drawing tons of pixels on top of it
        // since most of the skybox wont be visible most of the time
        cubemap_shader.use();
        skybox.set_uniforms(cubemap_shader, camera_e);
        skybox.render();

        msg_bus.process();

        screen.swap_buffers();

        // TODO: max frame rate. if there is still time in this frame use a wait function here
    }

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
