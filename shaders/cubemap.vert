#version 330 core

layout (location = 0) in vec3 vertex_pos;

out vec3 tex_coords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    tex_coords = vertex_pos;
    vec4 pos = projection * view * vec4(vertex_pos, 1.0);
    gl_Position = pos.xyww; // z component is always w so that these skybox pixels are always the farthest aways than any other
}
