#version 330 core

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 in_texcoord;

out vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    texcoord = in_texcoord;

    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}
