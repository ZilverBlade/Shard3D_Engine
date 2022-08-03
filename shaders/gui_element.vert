#version 450

#extension GL_EXT_shader_explicit_arithmetic_types_int64  : enable

#define maxLayers 128.0

const vec2 OFFSETS_ANCHOR_TOPLEFT[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 0.0),
  vec2(0.0, -1.0),
  vec2(0.0, -1.0),
  vec2(-1.0, 0.0),
  vec2(0.0, 0.0)
);
const vec2 OFFSETS_ANCHOR_TOPRIGHT[6] = vec2[](
  vec2(0.0, -1.0),
  vec2(0.0, 0.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(0.0, 0.0),
  vec2(1.0, 0.0)
);
const vec2 OFFSETS_ANCHOR_BOTTOMLEFT[6] = vec2[](
  vec2(-1.0, 0.0),
  vec2(-1.0, 1.0),
  vec2(0.0,	0.0),
  vec2(0.0,	0.0),
  vec2(-1.0, 1.0),
  vec2(0.0, 1.0)
);
const vec2 OFFSETS_ANCHOR_BOTTOMRIGHT[6] = vec2[](
  vec2(0.0, 0.0),
  vec2(0.0, 1.0),
  vec2(1.0,	0.0),
  vec2(1.0, 0.0),
  vec2(0.0, 1.0),
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
	vec2 position;
	float zPos;
	float rotation;
	vec2 scale;
	vec2 mousePos;
	float ar;
	int anchorType;
	uint64_t id;
} push;

vec2 rotate(vec2 v, float a) {
	const float s = sin(a);
	const float c = cos(a);
	const mat2 m = mat2(c, s, -s, c);
	return v * m;
}
void main(){
	vec2 fragOffset; // TODO: Change to sum operator (OFFSETS[gl_VertexIndex] + push.anchorOffset)
	if		(push.anchorType == 0) fragOffset = OFFSETS_ANCHOR_TOPLEFT		[gl_VertexIndex];
	else if (push.anchorType == 1) fragOffset = OFFSETS_ANCHOR_TOPRIGHT		[gl_VertexIndex];
	else if (push.anchorType == 2) fragOffset = OFFSETS_ANCHOR_BOTTOMLEFT	[gl_VertexIndex];
	else if (push.anchorType == 3) fragOffset = OFFSETS_ANCHOR_BOTTOMRIGHT	[gl_VertexIndex];

	fragUV = UV[gl_VertexIndex];
	id = push.id;
	mousePos = push.mousePos;
	vec2 shape = rotate(fragOffset * push.scale.xy, push.rotation);
	gl_Position = vec4(vec2(shape.x, shape.y * push.ar) + push.position,  0.9999 - push.zPos / maxLayers, 1.0);
}