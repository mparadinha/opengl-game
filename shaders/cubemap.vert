#version 330 core

layout (location = 0) in vec3 vertex_pos;

out vec3 tex_coords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    tex_coords = vertex_pos;
    vec4 pos = projection * view * vec4(vertex_pos, 1.0);

    // make z always 1.0 (same as w) so that cubemap fragment shader
    // always fails the depth test on pixels that have something in front
    // note: we can do this because we're rendering the cubemap after everything else
    gl_Position = pos.xyww;
}
