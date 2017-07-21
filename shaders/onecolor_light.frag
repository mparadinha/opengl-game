#version 320 es

// in opengl ES we need to specify a precision for types
precision mediump float;

in vec3 normal;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

uniform vec3 diffuse_color;

uniform Light light;

out vec4 FragColor;

void main() {
    vec3 normal = normalize(normal);

    // ambient part of light
    vec3 ambient = light.ambient * diffuse_color;

    // diffuse part
    float angle_diff = max(dot(normal, light.direction), 0.0);
    vec3 diffuse = angle_diff * (light.diffuse * diffuse_color);

    vec3 final_color = (diffuse + ambient);
    FragColor = vec4(final_color, 0.8);
}
