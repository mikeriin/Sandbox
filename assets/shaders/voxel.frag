#version 460 core


layout(location = 0) out vec4 FragColor;

layout(location = 0) in float inAO;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPos;
layout(location = 3) in vec2 inUV;
layout(location = 4) smooth in vec3 inViewPosition;


uniform sampler2D u_Tex;


const vec3 ambientColor = vec3(0.675, 0.529, 0.773);
const float ambientStrength = 0.25;
const vec3 sunlightColor = vec3(0.992, 0.984, 0.827);

const vec3 sunlightDirection = vec3(0.3618, 0.9044, 0.2261);
const vec3 color = vec3(0.95);


const vec3 fogColor = vec3(0.459, 0.416, 0.714);
const float fogDensity = 0.005;
const int fogPower = 5;


void main()
{
	float ao = 1.0 - inAO;
	float ao3 = ao * ao * ao;
	float inv = 1.0 - ao3;
	float scale = mix(1.0, ao, inv);

	float ambFac = mix(1.0, scale, 0.8);
	vec3 ambient = ambientColor * ambientStrength * ambFac;

	float diffAO = mix(1.0, ao, 0.75);
  float diff   = max(dot(inNormal, sunlightDirection), 0.0) * diffAO;
  vec3 diffuse = diff * sunlightColor;
	
	vec3 lit = ambient + diffuse;
  lit = (lit - 0.5) * 1.04 + 0.5;
  lit = clamp(lit, 0.0, 1.0);


	float fogDistance = length(inViewPosition);
	float fogFactor = exp(-pow(fogDensity * fogDistance, fogPower));
	float fog = clamp(1.0 - fogFactor, 0.0, 1.0);


	//vec3 finalColor = mix(sqrt(lit) * color, inPos / 32, 0.25);

	vec4 diffuseTex = texture(u_Tex, inUV);
	vec3 finalColor = sqrt(lit) * diffuseTex.rgb;

	FragColor = vec4(mix(finalColor, fogColor, fog), 1.0);
}