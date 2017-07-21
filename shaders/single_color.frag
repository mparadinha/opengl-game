#version 320 es

// in opengl ES we need to specify a precision for types
precision mediump float;

out vec4 FragColor;

uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0);
}
