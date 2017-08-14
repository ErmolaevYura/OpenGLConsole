#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 coords;

out vec2 texCoords;

uniform mat4 projection;
uniform mat4 model;

void main(void) {
    gl_Position = projection * model * vec4(position, 0.0f, 1.0f);
    texCoords = coords;
}