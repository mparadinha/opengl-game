#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "display.h"

#include "input.h"
#include "shader.h"
#include "game_object.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "signals.h"
#include "message_bus.h"
#include "cubemap.h"
#include "terrain.h"
#include "quad.h"

static const unsigned int WINDOW_WIDTH = 1200;
static const unsigned int WINDOW_HEIGHT = 900;

int main() {
    MessageBus msg_bus = MessageBus();
    Display screen = Display(WINDOW_WIDTH, WINDOW_HEIGHT, "TITLE");

    // shaders
    Shader terrain_shader = Shader("terrain.vert", "terrain.frag");
    terrain_shader.set_uniform("diffuse", 0);

    Shader cubemap_shader = Shader("cubemap.vert", "cubemap.frag");
    cubemap_shader.set_uniform("skybox", 0);

    Shader sun_shader = Shader("onecolor_light.vert", "onecolor_light.frag");
    sun_shader.set_uniform("light.direction", glm::vec3(-1));
    sun_shader.set_uniform("light.diffuse", glm::vec3(0.7f));
    sun_shader.set_uniform("light.ambient", glm::vec3(0.3f));

    // systems
    Camera camera = Camera(
        glm::vec3(0.0f, 1.8f, 0.0f), 90.0f, -25.0f,
        55.0f, WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f); // start camera looking at -z direction

    Input& input = Input::get_instance();
    input.init(screen.window, &camera, &msg_bus);

    // entities
    glBindVertexArray(0);
    Terrain terrain;

    CubeMap skybox = CubeMap("res/skybox/skybox", "jpg");

    GameObject dude("res/nanosuit/nanosuit.obj");
    dude.scale(glm::vec3(0.2f));

    // add systems to the message bus list
    msg_bus.add_system(&camera);
    msg_bus.add_system(&input);


    float dt, time, last_time = glfwGetTime();
    message_t new_frame_msg = {NEW_FRAME, {0}};
    bool show_fps = false;
    int frames_passed = 0;
    while(!input.should_close()) {
        // calculate delta time for this frame
        time = glfwGetTime();
        dt = time - last_time;
        last_time = time;

        // TODO: make this appear as text on screen
        frames_passed++;
        if(show_fps && frames_passed == 100) {
            std::cout << "FPS: " << 1/dt << std::endl;
            frames_passed = 0;
        }

        screen.clear();

        // post message to all systems that a new frame has started
        new_frame_msg.data.x = dt;
        msg_bus.add_message(new_frame_msg, PRIORITY_NOW);
        
        // draw objects
        terrain_shader.use();
        camera.set_all_uniforms(terrain_shader);
        terrain.render();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        sun_shader.use();
        camera.set_all_uniforms(sun_shader);

        sun_shader.set_uniform("diffuse_color", glm::vec3(1, 0, 0));
        dude.render(sun_shader);

        // draw skybox last so that we dont end up drawing tons of pixels on top of it
        // since most of the skybox wont be visible most of the time
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        cubemap_shader.use();
        skybox.set_uniforms(cubemap_shader, camera);
        skybox.render();

        msg_bus.process();

        screen.swap_buffers();

        // TODO: max frame rate. if there is still time in this frame use a wait function here
    }

    return 0;
}
