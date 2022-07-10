#version 450
// screen view aligned
#extension GL_KHR_vulkan_glsl : enable

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
);
const vec2 UV[6] = vec2[](
  vec2(1.0, 1.0),
  vec2(1.0, 0.0),
  vec2(0.0, 1.0),
  vec2(0.0, 1.0),
  vec2(1.0, 0.0),
  vec2(0.0, 0.0)
);


layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
} ubo;

layout(push_constant) uniform Push {
	vec4 translation;
	vec4 rotation;
	vec4 scale;
} push;

void main(){
	fragUV = UV[gl_VertexIndex];

	vec4 texInCameraSpace = ubo.view * push.translation;

	gl_Position = ubo.projection * positionInCameraSpace;
}