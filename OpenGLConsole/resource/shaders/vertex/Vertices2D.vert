#version 330
layout(location = 0) in vec3 position;

uniform mat4 vp;
uniform mat4 model;

// to fragment shader
out VertexFS
{
	vec3 Position;
} vertfs;

void main() {
	gl_Position = vp * model * vec4(position, 1.0f);
	gl_PointSize = 10.0;
	vertfs.Position = vec3(model * vec4(position, 1.0f));
}
