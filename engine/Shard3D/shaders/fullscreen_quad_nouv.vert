#version 450

// post processing
const vec2 OFFSETS[6] = vec2[](
	vec2(1.0, -1.0),
	vec2(1.0, 1.0),
	vec2(-1.0, -1.0),
	vec2(-1.0, -1.0),
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0)
	);


void main() {
	vec2 fragOffset = OFFSETS[gl_VertexIndex];
	gl_Position = vec4(fragOffset, 0.0, 1.0);
}