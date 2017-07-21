#version 320 es

layout (location = 0) in vec3 vertex_pos;

out vec3 tex_coords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    tex_coords = vertex_pos;
    gl_Position = projection * view * vec4(vertex_pos, 1.0);
}  
