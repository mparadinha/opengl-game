#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 color;

out vec4 frag_color;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0); 
    frag_color = color;
}
