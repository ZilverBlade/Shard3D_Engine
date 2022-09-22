#version 450
#extension GL_GOOGLE_include_directive : enable

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;

layout (location = 0) out vec4 outNormal;
layout (location = 1) out vec4 outMaterialDiffuse;
layout (location = 2) out vec4 outMaterialParam;
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE
// maybe maybe
layout (location = 3) out vec4 outMaterialEmissiveColor;
#endif

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
layout(set = 1, binding = 1) uniform MaterialFactor{
	vec4 diffuse;
	float specular;
	float shininess;
	float metallic;
} factor;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE
layout(set = 1, binding = 1) uniform MaterialFactor{
	vec4 emissive;
} factor;
#endif

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE
layout(set = 2, binding = 0) uniform sampler2D tex_emission;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
layout(set = 2, binding = 1) uniform sampler2D tex_diffuse;
layout(set = 2, binding = 2) uniform sampler2D tex_specular;
layout(set = 2, binding = 3) uniform sampler2D tex_shininess;
layout(set = 2, binding = 4) uniform sampler2D tex_metallic;
layout(set = 2, binding = 5) uniform sampler2D tex_normal;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED
layout(set = 2, binding = 6) uniform sampler2D tex_mask;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
layout(set = 2, binding = 8) uniform sampler2D tex_clearcoat;
#endif

// shader code
void main(){
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED
	if (texture(tex_mask, fragUV).x < 0.5) discard;
#endif
	const vec3 fragColor = 
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	texture(tex_diffuse, fragUV).xyz * factor.diffuse.xyz;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE
	texture(tex_emission, fragUV).xyz * factor.emission.xyz;
#endif

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	const float material_specular = texture(tex_specular, fragUV).x * factor.specular;
	const float material_shininess = texture(tex_shininess, fragUV).x * factor.shininess;
	const float material_metallic = texture(tex_metallic, fragUV).x * factor.metallic;

const float material_clearcoat =
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
	 texture(tex_clearcoat, fragUV).x * factor.clearcoat
#else
	0.0
#endif
;

	// // obtain normal from normal map in range [0,1]
	// vec3 normal = texture(tex_normal, fragUV).xyz;
   //// transform normal vector to range [-1,1]
   // normal = normalize(normal * 2.0 - 1.0);   

	const vec3 N = normalize(fragNormalWorld);
	
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	outMaterialDiffuse = vec4(fragColor, 1.0);
	outNormal = vec4(N, 0.0); 
	outMaterialParam = vec4(material_specular, material_shininess, material_metallic, material_clearcoat);
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE
	outMaterialEmissiveColor = vec4(fragColor, opacity);
#endif
}