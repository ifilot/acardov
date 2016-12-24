#version 330 core

in vec3 position;
in vec3 normal;

out vec3 eye_cameraspace;
out vec3 lightdirection_cameraspace;
out vec3 normal_cameraspace;
out vec3 position0;
out vec3 col;

uniform mat4 mvp;
uniform mat4 view;
uniform vec3 light;
uniform vec3 color;

void main() {
    col = color;

    vec3 position_cameraspace = (view * vec4(position, 1.0)).xyz;
    eye_cameraspace = vec3(0,0,0) - position_cameraspace;

    vec3 light_cameraspace = (view * vec4(light, 0)).xyz;
    lightdirection_cameraspace = light_cameraspace + eye_cameraspace;

    normal_cameraspace = (view * vec4(normal, 1.0)).xyz;

    gl_Position = mvp * vec4(position, 1.0);
}
