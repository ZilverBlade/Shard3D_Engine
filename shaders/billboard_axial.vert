#version 450
#extension GL_KHR_vulkan_glsl : enable

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);
const vec2 UV[6] = vec2[](
  vec2(1.0, 1.0),
  vec2(1.0, 0.0),
  vec2(0.0, 1.0),
  vec2(0.0, 1.0),
  vec2(1.0, 0.0),
  vec2(0.0, 0.0)
);

layout (location = 0) out vec2 fragOffset;
layout (location = 1) out vec2 fragUV;

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

layout(push_constant) uniform Push {
	vec4 translation;
	vec4 rotation;
	vec4 scale;
} push;

void main(){
	fragOffset = OFFSETS[gl_VertexIndex];
	fragUV= UV[gl_VertexIndex];
	
	vec4 texInCameraSpace = ubo.view * push.translation;
	vec4 positionInCameraSpace = texInCameraSpace + push.scale * vec4(fragOffset, 0.0, 0.0);

	gl_Position = ubo.projection * positionInCameraSpace;
}