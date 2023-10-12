#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;
float scale = .33f;
uniform float iTime;
float speed = 3;

void main(){
	float time = iTime * speed;
	vec3 pos = vec3( 
		(vPos.x + (cos(time)) + (cos(time*5))*.05f + (cos(time*.5f))),
		(vPos.y + abs((sin(time))) + (sin(time*5))*.05f +(sin(time*.5f))),
		0);
	UV = vUV; 
	gl_Position = vec4(pos*scale,1.0);
}