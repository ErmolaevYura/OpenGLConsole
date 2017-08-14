#version 400

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

/*
in int gl_VertexID;
in int gl_InstanceID;

// to geometric shader
out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};
out int gl_PrimitiveID;
out int gl_Layer;
out int gl_ViewportIndex;	// only for OpenGL 4.1+
*/

smooth out vec3 Position; 
smooth out vec3 Normal;
smooth out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	//TexCoords = vec2(texCoord.x, 1.0f - texCoord.y);
    Position = vec3(model * vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords;
}