#version 450
// screen view aligned
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

layout (location = 0) out vec2 fragUV;

layout(push_constant) uniform Push {
	vec2 translation;
	vec2 scale;
} push;

void main(){
	vec2 fragOffset = OFFSETS[gl_VertexIndex];
	fragUV = UV[gl_VertexIndex];

	gl_Position = vec4(fragOffset * push.scale.xy + push.translation.xy, 0.0, 1.0);
}