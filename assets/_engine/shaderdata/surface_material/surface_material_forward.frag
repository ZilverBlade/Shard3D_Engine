#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_KHR_vulkan_glsl : enable

#include "surface_lighting.glsl"

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout(set = 2, binding = 1) uniform MaterialFactor{
	vec4 diffuse;
	float specular;
	float shininess;
	float metallic;
} factor;

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE
layout(set = 3, binding = 0) uniform sampler2D tex_emission;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
layout(set = 3, binding = 1) uniform sampler2D tex_diffuse;
layout(set = 3, binding = 2) uniform sampler2D tex_specular;
layout(set = 3, binding = 3) uniform sampler2D tex_shininess;
layout(set = 3, binding = 4) uniform sampler2D tex_metallic;
layout(set = 3, binding = 5) uniform sampler2D tex_normal;
#endif       
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED
layout(set = 3, binding = 6) uniform sampler2D tex_mask;
#endif
layout(set = 3, binding = 7) uniform sampler2D tex_opacity;
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
layout(set = 3, binding = 8) uniform sampler2D tex_clearcoat;
#endif

// shader code
void main(){
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED
	if (texture(tex_mask, fragUV).x < 0.5) discard;
#endif

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE
	const vec3 emissive = texture(tex_emission, fragUV).xyz * factor.emission.xyz;
#endif

ShadedPixelInfo pixel;

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	pixel.diffuse = texture(tex_diffuse, fragUV).xyz * factor.diffuse.xyz;
	pixel.specular = texture(tex_specular, fragUV).x * factor.specular;
	pixel.shininess = texture(tex_shininess, fragUV).x * factor.shininess;
	pixel.metallic = texture(tex_metallic, fragUV).x * factor.metallic;
	
	// // obtain normal from normal map in range [0,1]
	// vec3 normal = texture(tex_normal, fragUV).xyz;
   //// transform normal vector to range [-1,1]
   // normal = normalize(normal * 2.0 - 1.0);   


	pixel.positionWorld = fragPosWorld;
	pixel.normal = normalize(fragNormalWorld);
#endif
	
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
	pixel.clearcoat = texture(tex_clearcoat, fragUV).x * factor.clearcoat;
#endif	
	const vec3 lightOutput = calculateLight(pixel);

const float opacity = texture(tex_opacity, fragUV).x * factor.diffuse.w;
	outColor = vec4(lightOutput
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE
	+ emissive
#endif
	, opacity);
}