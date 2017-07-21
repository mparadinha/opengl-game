#version 130

attribute vec3 vertex_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}
