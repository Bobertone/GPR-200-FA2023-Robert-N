#version 450
out vec4 FragColor;

in Surface{
	vec2 UV; //Per-fragment interpolated UV
	vec3 WorldPosition; //Per-fragment interpolated world position
	vec3 WorldNormal; //Per-fragment interpolated world normal
}fs_in;

struct Light{
	vec3 position;
	vec3 color;
};

#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];

uniform vec3 _camPosition;
uniform int _lightCount;

uniform float _ambientK; //Ambient coefficient (0-1)
uniform float _diffuseK; //Diffuse coefficient (0-1)
uniform float _specularK; //Specular coefficient (0-1)
uniform float _shininess; //Shininess

uniform sampler2D _Texture;

void main(){
	FragColor = texture(_Texture,fs_in.UV);
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 viewDirection = normalize(_camPosition - fs_in.WorldPosition);

	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0); 
	vec3 specular = vec3(0.0);

	for(int i = 0; i < _lightCount; i++){
		vec3 lightDir = normalize(_Lights[i].position - fs_in.WorldPosition);
		diffuse += _diffuseK * _Lights[i].color * max(dot(normal, lightDir), 0.0);
	
		vec3 reflectDirection = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), _shininess);
		specular += _specularK * spec * _Lights[i].color;

		ambient += _ambientK * _Lights[i].color;
	}
	vec4 texColor = texture(_Texture, fs_in.UV);
	vec3 resultColor = texColor.rgb *(ambient + diffuse + specular);
	FragColor = vec4(resultColor, texColor.a);

}