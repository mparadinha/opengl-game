#version 330 core

in vec3 normal;
in vec3 color;

out vec4 FragColor;

const float ambient = 0.4;
const vec3 sun = normalize(vec3(1, -1, 1));

void main() {
    float diffuse = max(dot(normal, sun), 0.0);
    FragColor = vec4((diffuse + ambient) * color, 1);
}
