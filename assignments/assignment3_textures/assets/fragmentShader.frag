#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _bgTexture;
uniform sampler2D _noiseTexture;
uniform float iTime;


void main(){

	float noise = texture(_noiseTexture,UV).r;
	vec2 uv = UV + noise * 0.1f * sin(iTime);
	vec4 colorA = texture(_bgTexture,uv);
	//vec4 colorB = texture(_SmileyTexture,uv);
	//vec3 color = mix(colorA.rgb,colorB.rgb,colorB.a * 0.5);


	FragColor = colorA;
}
