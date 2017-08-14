#version 410 core

in VertexFS
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
} vert;

uniform sampler2D diffuse;
uniform sampler2D heightmap;

void main() {
	gl_FragColor = texture(diffuse, vert.TexCoords) * (1-texture(heightmap, vert.TexCoords).r);
	gl_FragColor.a = 1.0f;
}