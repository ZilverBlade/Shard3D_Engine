#version 450

layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D uiTex;

void main()	{
	if (texture(uiTex, fragUV).w < 0.001) {
		discard;
	}
	outColor = vec4(texture(uiTex, fragUV).xyz, texture(uiTex, fragUV).w);
}