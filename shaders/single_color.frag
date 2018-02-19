#version 330 core

// in opengl ES we need to specify a precision for types
precision mediump float;

in vec3 normal;

out vec4 FragColor;

uniform vec3 color;

void main() {
    //FragColor = vec4(color, 1.0);
    FragColor = vec4(normal, 1.0);
}
