#include <iostream>

#include "glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "display.h"
#include "input.h"
#include "shader.h"
#include "game_object.h"
#include "camera.h"
#include "model.h"
#include "signals.h"
#include "message_bus.h"
#include "cubemap.h"
#include "terrain.h"
<<<<<<< HEAD
=======
#include "quad.h"
>>>>>>> b5c8cdb2c781d3ccbaaee45fd1b9b17e34a3ff31
#include "player.h"

static const unsigned int WINDOW_WIDTH = 1200;
static const unsigned int WINDOW_HEIGHT = 900;

int main() {
    MessageBus msg_bus = MessageBus();
    Display screen = Display(WINDOW_WIDTH, WINDOW_HEIGHT, "TITLE");

    // shaders
    Shader terrain_shader("terrain.vert", "terrain.frag");
    terrain_shader.set_uniform("diffuse", 0);

    Shader cubemap_shader("cubemap.vert", "cubemap.frag");
    cubemap_shader.set_uniform("skybox", 0);

    Shader color("single_color.vert", "single_color.frag");
    color.set_uniform("color", glm::vec3(1, 0, 0));

    Shader sun_shader = Shader("onecolor_light.vert", "onecolor_light.frag");
    sun_shader.set_uniform("light.direction", glm::vec3(-1));
    sun_shader.set_uniform("light.diffuse", glm::vec3(0.7f));
    sun_shader.set_uniform("light.ambient", glm::vec3(0.3f));

    // systems
    Camera camera(&msg_bus, (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT);
<<<<<<< HEAD
=======

>>>>>>> b5c8cdb2c781d3ccbaaee45fd1b9b17e34a3ff31
    Input input(screen.window, &msg_bus);

    // entities
    Terrain terrain;
    CubeMap skybox("res/skybox/skybox", "jpg");
    Player player(&msg_bus, "res/scooter/scooter.obj", &camera);
<<<<<<< HEAD
    GameObject axis("res/arrow/arrow.obj");
=======
    GameObject scooter("res/scooter/scooter.obj");
    scooter.translate(glm::vec3(0, 2, 0));
>>>>>>> b5c8cdb2c781d3ccbaaee45fd1b9b17e34a3ff31

    float dt, time, last_time = glfwGetTime();
    message_t new_frame_msg = {NEW_FRAME, {0}};
    bool show_fps = false;
    int frames_passed = 0;
    while(!input.should_close()) {
        glfwPollEvents();

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
        new_frame_msg.data[0] = dt;
        msg_bus.add_message(new_frame_msg, PRIORITY_NOW);
        
        // draw objects
        terrain_shader.use();
        camera.set_uniforms(terrain_shader);
        terrain.render();

<<<<<<< HEAD
        sun_shader.use();
        camera.set_uniforms(sun_shader);
        sun_shader.set_uniform("diffuse_color", glm::vec3(1, 0, 0)); 
        player.render(sun_shader);

        sun_shader.set_uniform("diffuse_color", glm::vec3(0, 1, 0));
        axis.render(sun_shader);
=======
        camera.set_uniforms(sun_shader);
        player.render(sun_shader);
        scooter.render(sun_shader);
>>>>>>> b5c8cdb2c781d3ccbaaee45fd1b9b17e34a3ff31

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
