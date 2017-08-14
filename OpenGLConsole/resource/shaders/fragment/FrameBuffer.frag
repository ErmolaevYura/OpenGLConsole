#version 150

in vec2 Texcoord;

uniform sampler2D tex;

void main()
{
    gl_FragColor = texture(tex, Texcoord);
}