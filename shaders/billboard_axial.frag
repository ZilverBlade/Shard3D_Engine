#version 450

layout (location = 0) in vec2 fragOffset;
layout (location = 1) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

struct Pointlight {
	vec4 position;
	vec4 color;
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
	float specularMod;
};
struct Spotlight {
	vec4 position;
	vec4 color;
	vec4 direction; // (ignore w)
	vec2 angle; //outer, inner
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
	float specularMod;
};
struct DirectionalLight {
	vec4 position;
	vec4 color;
	vec4 direction; //	directional (ignore w)
	float specularMod;	
};

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;

	vec4 ambientLightColor;			//	sky/ambient
	
	Pointlight pointlights[20];
	Spotlight spotlights[20];
	DirectionalLight directionalLights[6];
	int numPointlights;
	int numSpotlights;
	int numDirectionalLights;
} ubo;

layout (set = 1, binding = 0) uniform sampler2D billboardTex;

layout(push_constant) uniform Push {
	vec4 translation;
	vec4 rotation;
	vec4 scale;
} push;

void main()	{
	if (texture(billboardTex, fragUV).w == 0) {
		discard;
	}
	outColor = vec4(texture(billboardTex, fragUV).xyz, texture(billboardTex, fragUV).w);
}