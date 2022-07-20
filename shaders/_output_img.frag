#version 450
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 10) uniform sampler2D uiTex;

void main()	{
	outColor = vec4(texture(uiTex, fragUV).xyz, 1.0);
}