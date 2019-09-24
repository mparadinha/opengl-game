#version 330 core

layout (location = 0) in vec2 vert_pos;
layout (location = 1) in vec2 tex_coord;

uniform mat4 view;
uniform mat4 projection;

out vec2 tex_pass;

void main() {
    ////gl_Position = view * projection * vec4(vert_pos, 1, 1);
    gl_Position = vec4(vert_pos, 0, 1);
    tex_pass = tex_coord;
}
