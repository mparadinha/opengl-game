#version 320 es

// in opengl ES we need to specify a precision for types
precision mediump float;

in vec2 texcoord;
in vec3 normal;
in vec3 fragment_pos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;
const float shininess = 32.0;

uniform Light light;

uniform vec3 view_pos;

out vec4 FragColor;

void main() {
    vec3 diffuse_color = vec3(texture(diffuse_tex, texcoord));

    // ambient part of light
    vec3 ambient = light.ambient * diffuse_color;

    // diffuse part
    vec3 normal = normalize(normal);
    vec3 light_dir = normalize(light.position - fragment_pos);
    float angle_diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = angle_diff * (light.diffuse * diffuse_color);

    // specular light
    vec3 view_dir = normalize(view_pos - fragment_pos);
    vec3 reflection_dir = reflect(-light_dir, normal);
    float specular_spread = pow(max(dot(view_dir, reflection_dir), 0.0),
        shininess);
    vec3 specular = light.specular * specular_spread *
        vec3(texture(specular_tex, texcoord));

    // calculate attenuation for more distante objects
    float distance = length(light.position - fragment_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance
        + light.quadratic * pow(distance, 2.0));

    vec3 final_color = (diffuse + ambient + specular);
    FragColor = vec4(final_color, 1.0);
}
