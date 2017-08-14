#version 330

in VertexFS
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
} vert;

uniform sampler2D diffuse;
uniform vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void main() {
	gl_FragColor = color * texture(diffuse, vert.TexCoords);
}