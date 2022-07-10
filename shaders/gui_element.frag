#version 450

layout (location = 0) in vec2 fragUV;
layout (location = 1) in flat int id;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 2) uniform sampler2D uiTex;

void main()	{
	if (texture(uiTex, fragUV).w < 0.001) {
		discard;
	}
	outColor = vec4(texture(uiTex, fragUV).xyz, texture(uiTex, fragUV).w);
}