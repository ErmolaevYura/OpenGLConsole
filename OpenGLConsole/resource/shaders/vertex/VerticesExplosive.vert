#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// to next shader
out VertexCS 
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
} vertns;


void main(){
    gl_Position =  projection * view * model * vec4(position, 1.0f);
    vertns.Position = vec3(view * model * vec4(position, 1.0f));
	vertns.Normal = mat3(transpose(inverse(model))) * normal;
	vertns.TexCoords = texcoords;
}
