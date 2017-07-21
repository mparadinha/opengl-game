#version 320 es

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}
