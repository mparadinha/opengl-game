#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 axis_color;

uniform mat4 view;

out vec3 color;

void main() {
    gl_Position = view * vec4(pos, 1.0);
    color = axis_color;
}
