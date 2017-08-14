#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoords;

uniform mat4 model;

// to control shader
out VertexCS 
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
} vertcs;

void main(){
    gl_Position =  vec4(position, 1.0f);
    vertcs.Position = position;
	vertcs.Normal = normal;
	vertcs.TexCoords = texcoords;
}
