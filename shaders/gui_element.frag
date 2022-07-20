#version 450
#extension GL_EXT_shader_explicit_arithmetic_types_int64  : enable
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 2) uniform sampler2D uiTex;
layout (location = 1) in vec2 mousePos;
layout (location = 2) in flat uint64_t id;

layout (set = 1, binding = 10) writeonly buffer IdOutput {
	uint64_t selectedID;
} idOut;

void main()	{
	if (texture(uiTex, fragUV).w < 0.001) 
		discard; // discard before mouse picking operation as we dont want to select masked out elements
	float len = length( mousePos - gl_FragCoord.xy);
	if(len < 1)
		idOut.selectedID = id; 
	outColor = vec4(texture(uiTex, fragUV).xyz 
	* min((len/100), 1.0), // this multiplication is just for debugging purposes
	texture(uiTex, fragUV).w);
}