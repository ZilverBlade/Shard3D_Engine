#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_KHR_vulkan_glsl : enable

#include "global_ubo.glsl"
#include "surface_lighting.glsl"
#include "depth_to_worldpos.glsl"

layout (input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput inputDepth;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputNormal;
layout (input_attachment_index = 2, set = 1, binding = 2) uniform subpassInput inputMaterialDiffuse;
layout (input_attachment_index = 3, set = 1, binding = 3) uniform subpassInput inputMaterialParam;

layout (location = 0) out vec4 outColor;

// shader code
void main(){
	const vec4 elaborateMaterialData = subpassLoad(inputMaterialParam);
	const vec4 elaborateColorData= subpassLoad(inputMaterialDiffuse);
	ShadedPixelInfo pixel;
	pixel.diffuse = elaborateColorData.rgb;
	pixel.opacity = 1.0;
	pixel.positionWorld = WorldPosFromDepth(gl_FragCoord.xy, ubo.screenSize, subpassLoad(inputDepth).r, ubo.invProjection, ubo.invView);
	pixel.normal = subpassLoad(inputNormal).xyz;
	pixel.specular = elaborateMaterialData.r;
	pixel.shininess = elaborateMaterialData.g;
	pixel.metallic = elaborateMaterialData.b;

	const vec3 lightOutput = calculateLight(pixel);

	outColor = vec4(lightOutput, pixel.opacity); //RGBA
}