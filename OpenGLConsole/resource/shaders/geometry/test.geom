
#version 400

layout (triangles, invocations = 1) in;		// only OpenGL 4.0+
layout (triangle_strip, max_vertices = 3) out;

in gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[];
in int gl_PrimitiveIDIn;
in int gl_InvocationID;		/// for OpenGL 4.0+

void main() { }
