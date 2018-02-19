#version 330 core

const int MAX_BONES = 50; // max number of bones in a single mesh
const int MAX_WEIGHTS = 3; // each vertex can only be affected by 3 bones


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;
layout (location = 3) in ivec3 bone_ids;
layout (location = 4) in vec3 weights;

uniform mat4 bones_transforms[MAX_BONES]; 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 final_model_pos = vec4(0.0);

    for(int i = 0; i < MAX_WEIGHTS; i++) {
        mat4 pose_pos = bone_transforms[bone_ids[i]] * vec4(position, 1.0);
        final_model_pos += pose_pos * weights[i];
    }

    gl_Position = projection * view * model * final_model_pos;
}
