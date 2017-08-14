#version 400

in vec3 Normal;
in vec3 Position;

uniform vec3 viewPos;
uniform samplerCube tex;

void main()
{
	vec3 I = normalize(Position - viewPos);
	vec3 R = reflect(I, normalize(Normal));
	gl_FragColor = texture(tex, R);
}