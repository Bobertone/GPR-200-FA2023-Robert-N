#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _flowerTexture;

void main()
{
	vec4 colorA = texture(_flowerTexture, UV);
	FragColor = colorA;
}
