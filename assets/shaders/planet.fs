#version 330 core

in vec3 pos;
in vec2 texcoord;

out vec4 outcol;

uniform sampler2D text;

void main() {
    outcol = texture(text, texcoord);
}
