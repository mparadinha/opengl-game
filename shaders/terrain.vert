#version 320 es

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoords;

uniform mat4 view, projection;

out vec2 texcoords;

void main() {
    texcoords = vertex_texcoords;
    gl_Position = projection * view * vec4(vertex_pos, 1.0);
}
