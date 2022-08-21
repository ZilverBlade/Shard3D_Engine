#version 450

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;

layout (location = 0) out vec4 outColor;


layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;
} ubo;

layout(set = 1, binding = 1) uniform MaterialFactor{
	vec4 emissive;
} factor;

layout(set = 2, binding = 1) uniform sampler2D tex_emissive;

layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

// shader code
void main(){

	vec3 fragColor = texture(tex_emissive, fragUV).xyz * factor.emissive.xyz;

	outColor = vec4(fragColor, 1.0); //RGBA
}