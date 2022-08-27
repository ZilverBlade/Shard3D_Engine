#version 450

layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 1) uniform sampler2D billboardTex;

void main()	{
// clamp the "mask" value to be either 1 (visible) or 0 (discarded)
	if (texture(billboardTex, fragUV).w < 0.5) {
		discard;
	}
	outColor = vec4(texture(billboardTex, fragUV).xyz, texture(billboardTex, fragUV).w);
}