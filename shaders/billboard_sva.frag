#version 450

layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D billboardTex;

void main()	{
	if (texture(billboardTex, fragUV).w < 0.001) {
		discard;
	}
	outColor = vec4(texture(billboardTex, fragUV).xyz, texture(billboardTex, fragUV).w);
}