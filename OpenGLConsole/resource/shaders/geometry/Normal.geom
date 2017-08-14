#version 400

layout (triangles) in;		//invocations = 1 only OpenGL 4.0+
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 Normal;
} gs_in[];

const float MAGNITUDE = 0.4f;

void GenerateLine(int index) {
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].Normal, 0.0f) * MAGNITUDE;
	EmitVertex();
	EndPrimitive();
}

void main() {
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}
