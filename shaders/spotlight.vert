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

layout (location = 0) out vec2 fragOffset;

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

void main(){
	fragOffset = OFFSETS[gl_VertexIndex];
	
	vec4 lightInCameraSpace = ubo.view * push.position;
	vec4 positionInCameraSpace = lightInCameraSpace + 1 * vec4(fragOffset, 0.0, 0.0);

	gl_Position = ubo.projection * positionInCameraSpace;
}