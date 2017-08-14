#version 400

in vec3 TexCoords;

uniform samplerCube skybox;

void main(void) {
	gl_FragColor = texture(skybox, TexCoords);
}