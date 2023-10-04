#version 450
out vec4 FragColor;
in vec2 UV;
uniform vec3 _Color;
uniform float _Brightness;

uniform float xResolution;
uniform float yResolution;
uniform float iTime;

void main(){
	//FragColor = vec4(UV,0.0,1.0);
	
    // Normalized pixel coordinates (from 0 to 1)
    vec2 iResolution = vec2(xResolution,yResolution);
    vec2 myUV = gl_FragCoord.xy / iResolution.xy;

    vec3 sky = mix(vec3(1.0, 0.8, 0.0), vec3(0.9, 0.0, 0.15), myUV.y - sin(iTime) * 0.5);
    float mountains = 1.0 - step(((sin(myUV.x * 23.56) * 0.05) + (cos(myUV.x * 40.56) * 0.025) + (cos(myUV.x * 90.56) * 0.008) + 0.25), myUV.y);

    vec2 sun = (myUV * 2.0) - 1.0;
    sun.x *= iResolution.x / iResolution.y;
    float d = distance(sun, vec2(0.0, sin(iTime)));
    d = smoothstep(0.3, 0.4, d);

    // Blend sun with sky
    sky = mix(vec3(1.0, 1.0, 0.0), sky, d);
    // Blend dark gray mountains with sky and sun
    sky = mix(sky, vec3(0.2), mountains);

    // Output to screen
    FragColor = vec4(sky, 1.0);
}