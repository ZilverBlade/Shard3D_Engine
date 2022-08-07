#version 450
// editor only

const vec2 OFFSETS[6] = vec2[](
  vec2(1.0, -1.0),
  vec2(1.0, 1.0),
  vec2(-1.0, -1.0),
  vec2(-1.0, -1.0),
  vec2(1.0, 1.0),
  vec2(-1.0, 1.0)
);
const vec2 UV[6] = vec2[](
  vec2(1.0, 1.0),
  vec2(1.0, 0.0),
  vec2(0.0, 1.0),
  vec2(0.0, 1.0),
  vec2(1.0, 0.0),
  vec2(0.0, 0.0)
);

layout (location = 0) out vec2 fragUV;

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
} ubo;

layout(push_constant) uniform Push {
	vec4 translation;
} push;

void main(){
	vec2 fragOffset = OFFSETS[gl_VertexIndex];
	fragUV = UV[gl_VertexIndex];

	vec4 texInCameraSpace = ubo.view * push.translation;
	vec4 positionInCameraSpace = texInCameraSpace + 0.2 * vec4(fragOffset, 0.0, 0.0);

	gl_Position = ubo.projection * positionInCameraSpace;
}