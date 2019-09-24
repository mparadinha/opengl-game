#version 330 core

in vec2 tex_pass;

uniform sampler2D tex;

out vec4 FragColor;

void main() {
    vec4 texel_color = texture(tex, tex_pass);
    FragColor = vec4(1, 1, 1, texel_color.r);
}
