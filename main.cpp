#include <iostream>

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

#include "systems/renderer.h"
#include "systems/loader.h"
#include "systems/animator.h"

#include "components/component_enum.h"
#include "components/transformation.h"
#include "components/mesh.h"

static const unsigned int WINDOW_WIDTH = 1600;
static const unsigned int WINDOW_HEIGHT = 900;

void display_fps(float dt);

int main() {
    gltf_test();
    return 0;

    MessageBus msg_bus = MessageBus();
    Display screen = Display(WINDOW_WIDTH, WINDOW_HEIGHT, "TITLE");

    Renderer renderer(&msg_bus);
    Loader loader(&msg_bus);
    Animator animator(&msg_bus);

    mesh_t thot_mesh = loader.load_mesh("res/thot_dab.gltf");
    //loader.load_animation("res/thot_dab.gltf");

    transformation_t transform;
    transform.scaling = glm::mat4(1);
    transform.rotation = glm::mat4(1);
    transform.translation = glm::mat4(1);
    Entity thot_e;
    thot_e.components[TRANSFORMATION] = &transform;
    thot_e.components[MESH] = &thot_mesh;
    

    // shaders
    Shader terrain_shader("terrain");

    Shader default_shader("default");

    Shader cubemap_shader("cubemap");
    cubemap_shader.set_uniform("skybox", 0);

    Shader color("single_color");
    color.set_uniform("color", glm::vec3(1, 0, 0));

    Shader sun_shader = Shader("onecolor_light");
    sun_shader.set_uniform("light.direction", glm::vec3(-1));
    sun_shader.set_uniform("light.diffuse", glm::vec3(0.7f));
    sun_shader.set_uniform("light.ambient", glm::vec3(0.3f));

    // systems
    Camera camera(&msg_bus, (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT);
    Input input(screen.window, &msg_bus);

    // entities
    Terrain terrain;
    CubeMap skybox("res/skybox/skybox", "jpg");
    //GameObject nanodude("res/nanosuit/nanosuit.obj");
    GameObject thot("res/thot_dab.dae");
    thot.rotate(-90, glm::vec3(1, 0, 0));
    thot.translate(glm::vec3(5, 0, 0));
    //nanodude.scale(0.2f);
    Player player(&msg_bus, &thot, &camera, glm::vec3(0));
    Mesh cube = make_cube();

    float dt, time, last_time = glfwGetTime();
    message_t new_frame_msg = {NEW_FRAME, {0}};
    bool show_fps = false;
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
        
        // draw objects
        terrain_shader.use();
        camera.set_uniforms(terrain_shader);
        terrain.render();

        renderer.render(camera, thot_e);

        sun_shader.use();
        sun_shader.set_uniform("diffuse_color", glm::vec3(0.2f));
        camera.set_uniforms(sun_shader);
        player.render(sun_shader);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        sun_shader.use();
        camera.set_uniforms(sun_shader);
        auto m = glm::translate(glm::mat4(1), glm::vec3(1, 1, 0));
        sun_shader.set_uniform("model", m);
        sun_shader.set_uniform("diffuse_color", glm::vec3(1, 0, 0));
        cube.render();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // draw skybox last so that we dont end up drawing tons of pixels on top of it
        // since most of the skybox wont be visible most of the time
        cubemap_shader.use();
        skybox.set_uniforms(cubemap_shader, camera);
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
    if(frames == 100) {
        std::cout << "FPS: " << frames / time << std::endl;
        frames = 0;
        time = 0;
    }
    else {
        frames++;
        time += dt;
    }
}
