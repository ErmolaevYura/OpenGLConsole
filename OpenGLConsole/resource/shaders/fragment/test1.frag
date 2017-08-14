#version 330
 
/*
out vec4 FragColor;
in vec2 TexCoordOut;
uniform sampler2D diffuse;
void main()
{
    float Depth = texture(diffuse, TexCoordOut).x;
    Depth = 1.0 - (1.0 - Depth) * 25.0;
    FragColor = vec4(Depth);
}
layout(location = 0) out float fragmentdepth;
void main(){
	float near = 1.0f;
	float far = 100.0f;
    float z = gl_FragCoord.z * 2.0f - 1.0f;
    float depth = (2.0f * near * far) / (far + near - z * (far - near)) / far;
    fragmentdepth = depth;
}
*/
void main() 
{
	float near = 1.0f;
	float far = 100.0f;
    float z = gl_FragCoord.z * 2.0f - 1.0f;
    float depth = (45.0f * near * far) / (far + near - z * (far - near)) / far;
    gl_FragColor = vec4(vec3(z), 1.0f);
}