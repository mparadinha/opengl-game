#version 330

in vec3 color;

out vec4 FragColor;

const vec3 ambient = vec3(0.2);

void main() {
    FragColor = vec4(color + ambient, 1.0);
}
