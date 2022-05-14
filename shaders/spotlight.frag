#version 450

layout (location = 0) in vec2 fragOffset;
layout (location = 0) out vec4 outColor;


struct Pointlight {
	vec4 position;
	vec4 color;
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
};

struct Spotlight {
	vec4 position; //ignore w
	vec4 color;
	vec4 direction; //ignore w
	float outerAngle;
	float innerAngle;
	vec4 attenuationMod;
};

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;

	vec4 ambientLightColor;			//	sky/ambient
	//vec3 directionalLightDirection;	//	directional
	
	Pointlight pointlights[20];
	Spotlight spotlights[20];
	int numPointlights;
	int numSpotlights;

} ubo;

layout(push_constant) uniform Push {
vec4 position;
vec4 color;
vec4 direction;
float outerAngle;
float innerAngle;
vec4 attenuationMod;

} push;

void main()	{

	float dis = sqrt(dot(fragOffset, fragOffset));
	if (dis >= 1.0){
		discard;
	}
	outColor = vec4(push.color.xyz, 1.0);
}