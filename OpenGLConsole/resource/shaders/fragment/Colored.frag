#version 330

in VertexFS
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
} vert;

uniform vec4 color;

void main() {
	gl_FragColor = vec4(vert.Position.x * color.r, vert.Position.y * color.g, vert.Position.z * color.b, color.a);	
}