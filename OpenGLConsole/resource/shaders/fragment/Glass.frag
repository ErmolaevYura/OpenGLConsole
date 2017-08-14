#version 400

in vec3 Normal;
in vec3 Position;

uniform float ref;
uniform vec3 viewPos;
uniform samplerCube tex;

void main()
{
	float ratio = 1.0f / ref;
	vec3 I = normalize(Position - viewPos);
	vec3 R = refract(I, normalize(Normal), ratio);
	gl_FragColor = texture(tex, R);
}