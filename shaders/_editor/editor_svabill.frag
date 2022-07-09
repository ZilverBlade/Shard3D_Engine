#version 450

layout (location = 0) in vec2 fragOffset;
layout (location = 1) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 1) uniform sampler2D billboardTex;

layout(push_constant) uniform Push {
	vec4 translation;
} push;

void main()	{
	if (texture(billboardTex, fragUV).w < 0.001) {
		discard;
	}
	outColor = vec4(texture(billboardTex, fragUV).xyz, texture(billboardTex, fragUV).w);
}