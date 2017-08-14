#version 330

in vec2 texCoords;
out vec4 outputColor;

uniform sampler2D textSampler;
uniform vec4 fontColor;

void main(void) {
	vec4 textColor = texture2D(textSampler, texCoords);
	outputColor = fontColor * vec4(textColor.r, textColor.r, textColor.r, textColor.r);
}