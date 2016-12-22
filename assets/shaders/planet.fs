#version 330 core

in vec3 pos;
in vec3 col;

out vec4 outcol;

void main() {
    outcol = vec4(col,1.0);
}
