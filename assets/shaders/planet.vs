#version 330 core

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 pos;
out vec2 texcoord;
uniform mat4 mvp;

void main() {
    pos = position;
    texcoord = uv;
    gl_Position = mvp * vec4(position, 1.0);
}
