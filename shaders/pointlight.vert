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

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;

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

void main(){
	fragOffset = OFFSETS[gl_VertexIndex];
	
	vec4 lightInCameraSpace = ubo.view * push.position;
	vec4 positionInCameraSpace = lightInCameraSpace + push.radius * vec4(fragOffset, 0.0, 0.0);

	gl_Position = ubo.projection * positionInCameraSpace;
}