#ifndef FPS_CAMERA_H
#define FPS_CAMERA_H

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "message_bus.h"

enum mov_enum {
    FRONT, BACK, LEFT, RIGHT
};

typedef struct {
    glm::vec3 x, y, z;
} axis_system;

class CameraFPS {
public:
    CameraFPS(glm::vec3 pos, float pitch, float yaw, float fov, float ratio, float near, float far);

    void move(mov_enum direction);
    void move(float x, float y);
    void mouse_move(float xdiff, float ydiff);

    void set_all_uniforms(Shader& shader);
    void set_view_uniform(Shader& shader) {
        shader.set_uniform("view", &view[0][0]);
    };
    void set_projection_uniform(Shader& shader) {
        shader.set_uniform("projection", &projection[0][0]);
    };

    void toggle_style() { free = !free; std::cerr << "toggle free camera\n"; }

    void handle_message(message_t msg);

    void update(float dt);

    glm::vec3 get_pos() { return pos; }
    glm::mat4 get_view() { return view; }

    ~CameraFPS();

private:
    void update_vectors();
    void update_view();

    bool free = true;

    glm::mat4 view, projection;
    glm::vec3 pos, front, up, right;
    glm::vec4 direction; // front, back, right, left
    float fov;
    float speed = 10.0f;
    float sensitivity = 0.2f;
    float yaw, pitch;
};

#endif // include guard
