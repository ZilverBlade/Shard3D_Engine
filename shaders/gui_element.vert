#version 450

#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64  : enable

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
layout (location = 1) out vec2 mousePos;
layout (location = 2) out uint64_t id;

layout(push_constant) uniform Push {
	vec2 translation;
	vec2 scale;
	vec2 mousePos;
	float rotation;
	uint64_t id;
} push;
vec2 rotate(vec2 v, float a) {
	const float s = sin(a);
	const float c = cos(a);
	const mat2 m = mat2(c, -s, s, c);
	return m * v;
}
void main(){
	vec2 fragOffset = OFFSETS[gl_VertexIndex];
	fragUV = UV[gl_VertexIndex];
	id = push.id;
	mousePos = push.mousePos;
	gl_Position = vec4(rotate(fragOffset, push.rotation) * push.scale.xy + push.translation.xy, 0.0, 1.0);
}