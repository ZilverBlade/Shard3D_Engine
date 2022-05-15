#version 450

layout (location = 0) in vec2 fragOffset;
layout (location = 0) out vec4 outColor;


struct Pointlight {
	vec4 position;
	vec4 color;
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
};

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;

	vec4 ambientLightColor;			//	sky/ambient
	//vec3 directionalLightDirection;	//	directional
	
	Pointlight pointlights[20];
	int numLights;

} ubo;

layout(push_constant) uniform Push {
vec4 position;
vec4 color;
vec4 attenuationMod;
float radius;
} push;

const float M_PI = 3.1415926538;

void main()	{
	float dis = sqrt(dot(fragOffset, fragOffset));
	if (dis >= 1.0 || dis <= 0.33){
		discard;
	}
	float cosDist =  0.5 * (cos(dis * 3 * M_PI) + 1.0);
	outColor = vec4(push.color.xyz + cosDist, cosDist);
}