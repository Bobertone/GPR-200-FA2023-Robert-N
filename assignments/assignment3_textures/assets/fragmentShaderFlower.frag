#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _flowerTexture;
uniform float iTime;
float scale = 4;

void main()
{
	vec4 colorA = texture(_flowerTexture, UV*scale + sin(iTime) + cos(iTime));
	FragColor = colorA;
}
