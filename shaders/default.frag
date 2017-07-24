#version 330 core

// in opengl ES we need to specify a precision for types
precision mediump float;

in vec2 texcoord;

out vec4 FragColor;

uniform sampler2D diffuse;

void main() {
    FragColor = texture(diffuse, texcoord);
}
