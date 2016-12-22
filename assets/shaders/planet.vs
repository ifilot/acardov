#version 330 core

in vec3 position;
in vec3 normal;
in vec3 color;

out vec3 pos;
out vec3 col;
uniform mat4 mvp;

void main() {
    pos = position;
    col = color;
    gl_Position = mvp * vec4(position, 1.0);
}
