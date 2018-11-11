#version 330 core

const int MAX_BONES = 50; // max number of bones in a single mesh
const int MAX_WEIGHTS = 4; // each vertex can only be affected by 4 bones


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in ivec4 joints;
layout (location = 3) in vec4 weights;

out vec3 normal;
out vec3 color;

uniform mat4 joint_transforms[MAX_BONES]; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 total_pos = vec4(0.0);
    vec4 total_norm = vec4(0.0);
    vec4 norm_weights = normalize(weights);

    mat4 skin_matrix = mat4(0.0);
    for(int i = 0; i < MAX_WEIGHTS; i++) {
        skin_matrix += weights[i] * joint_transforms[joints[i]];
    }

    gl_Position = projection * view * model * skin_matrix * vec4(position, 1.0);

    // pass normal to fragment shader
    normal = norm;
    color = weights.xyz;
}
