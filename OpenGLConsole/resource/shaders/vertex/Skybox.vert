#version 400

layout(location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 vp;
uniform mat4 projection;

void main(void) {
	vec4 pos = vp * model * vec4(position, 1.0f);
	gl_Position = pos.xyww;
	TexCoords = position;
}