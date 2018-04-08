#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const vec3 light = vec3(1.0, -1.0, -1.0);
const vec3 c = vec3(1.0, 0.0, 0.0);

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0); 
    //color = c * max(dot(norm, light), 0.0);
    color = norm;
}
