#ifndef GLOBAL_UBO_GLSL
#define GLOBAL_UBO_GLSL
struct Pointlight {
	vec4 position;
	vec4 color;
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
	float specularMod;
	float radius;
};
struct Spotlight {
	vec4 position;
	vec4 color;
	vec4 direction; // (ignore w)
	vec2 angle; //outer, inner
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
	float specularMod;
	float radius;
};
struct DirectionalLight {
	vec4 position;
	vec4 color;
	vec4 direction; //	directional (ignore w)
	float specularMod;	
};

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 invProjection;
	mat4 view;
	mat4 invView;

	vec2 screenSize;

	vec4 ambientLightColor;			//	sky/ambient

	Pointlight pointlights[256];
	Spotlight spotlights[256];
	DirectionalLight directionalLights[16];
	int numPointlights;
	int numSpotlights;
	int numDirectionalLights;
} ubo;
#endif