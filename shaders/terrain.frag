#version 320 es

// in opengl ES we need to specify a precision for types
precision mediump float;

uniform sampler2D diffuse;

in vec2 texcoords;

out vec4 FragColor;

void main() {
    FragColor = texture(diffuse, texcoords);
}
