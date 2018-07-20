#version 330

in vec4 frag_color;

out vec4 FragColor;

const vec3 ambient = vec3(0.2);

void main() {
    FragColor = frag_color + vec4(ambient, 0.0);
}
