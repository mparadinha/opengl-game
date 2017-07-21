#version 320 es

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_norm;
layout (location = 2) in vec2 texture_pos;

out vec2 texcoord;
out vec3 normal;
out vec3 fragment_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main() {
    // stuff to pass onto fragment shader
    texcoord = texture_pos;
    normal = mat3(transpose(inverse(model))) * vertex_norm;
    fragment_pos = vec3(model * vec4(vertex_pos, 1.0));

    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}
