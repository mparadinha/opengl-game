#version 320 es

// in opengl ES we need to specify a precision for types
precision mediump float;

out vec4 FragColor;

in vec3 tex_coords;

uniform samplerCube skybox;

void main() {    
    FragColor = texture(skybox, tex_coords);
}
