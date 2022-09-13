// #version 450
#include "global_ubo.glsl"

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;

layout (location = 0) out vec4 outColor;


layout(set = 1, binding = 1) uniform MaterialFactor{
	vec4 diffuse;
	float specular;
	float shininess;
	float metallic;
} factor;

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_UNSHADED
layout(set = 2, binding = 0) uniform sampler2D tex_emission;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
layout(set = 2, binding = 1) uniform sampler2D tex_diffuse;
layout(set = 2, binding = 2) uniform sampler2D tex_specular;
layout(set = 2, binding = 3) uniform sampler2D tex_shininess;
layout(set = 2, binding = 4) uniform sampler2D tex_metallic;
layout(set = 2, binding = 5) uniform sampler2D tex_normal;
#endif
#ifdef S3DSDEF_SURFACE_MASKED_UNIFORMSAMPLER2D_EXT
layout(set = 2, binding = 6) uniform sampler2D tex_mask;
#endif
#ifdef S3DSDEF_SURFACE_TRANSLUCENT_UNIFORMSAMPLER2D_EXT
layout(set = 2, binding = 7) uniform sampler2D tex_opacity;
#endif
layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

float PseudoGeometrySchlickGGX(float NdotV, float shine)
{
    float r = (2.0 - shine);
    float k = r*r / 8;

    return NdotV / (1.0 - k) + 0.01;
}
float PseudoGeometrySmith(vec3 N, vec3 V, float shine)
{
    float NdotV = max(dot(N, V), 0.0);

    return PseudoGeometrySchlickGGX(NdotV, shine) * 5;
}

vec3 fresnelSchlick(float cosTheta, vec3 baseSpec) {
    return baseSpec + (1.0 - baseSpec) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float specularExponent(float dotNH, float shine, float PGS, float sourceRadius){
	return pow(max(dotNH, 0) * (1 + sourceRadius / 100), shine * 256 + 2) * PGS;
}

vec3 calculateLight(vec3 incolor, vec3 baseSpecular, vec3 normal, vec3 viewpoint, vec3 lightPos, vec4 l_Color, float l_Radius, vec3 m_Color, float m_Specular, float m_Shininess, float m_Metallic) {
	vec3 L = normalize(lightPos - fragPosWorld);
	vec3 H = normalize(L + viewpoint);

	float dist = length(lightPos - fragPosWorld);
	float attenuation = 1.0 / (dist * dist);
	vec3 radiance = l_Color.xyz * l_Color.w * attenuation;
	
	float diffuseLight = max(dot(normal, L), 0);	
	
	vec3 diffuse = incolor * radiance * diffuseLight;

	float G   = PseudoGeometrySmith(normal, viewpoint, m_Shininess);
	vec3 F = fresnelSchlick(clamp(dot(H, viewpoint), 0.0, 1.0), baseSpecular);
	float specularLight = specularExponent(dot(normal, H), m_Shininess, G, l_Radius);

	vec3 specular = m_Specular * radiance * specularLight;

	vec3 Kd = vec3(1.0);
	
	Kd *= 1.0 - m_Metallic;

	return Kd * diffuse + specular * max(F, 0);
}
	
// shader code
void main(){
#ifdef S3DSDEF_SURFACE_MASKEDFUNC
	if (texture(tex_mask, fragUV).x < 0.5) discard;
#endif
	const vec3 fragColor = 
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	texture(tex_diffuse, fragUV).xyz * factor.diffuse.xyz;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_UNSHADED
	texture(tex_emission, fragUV).xyz * factor.emission.xyz;
#endif

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	const float material_specular = texture(tex_specular, fragUV).x * factor.specular;
	const float material_shininess = texture(tex_shininess, fragUV).x * factor.shininess;
	const float material_metallic = texture(tex_metallic, fragUV).x * factor.metallic;
	

	// // obtain normal from normal map in range [0,1]
	// vec3 normal = texture(tex_normal, fragUV).xyz;
   //// transform normal vector to range [-1,1]
   // normal = normalize(normal * 2.0 - 1.0);   

	const vec3 N = normalize(fragNormalWorld);


	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 V = normalize(cameraPosWorld - fragPosWorld);

	vec3 lightOutput = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

	vec3 F0 = vec3(1.0); // base specular
	F0 = mix(F0, fragColor, material_metallic);

	for (int i = 0; i < ubo.numPointlights; i++) {
		Pointlight light = ubo.pointlights[i];

		lightOutput += 
		calculateLight(fragColor, F0, N, V, 
			light.position.xyz, light.color, light.radius, 
			fragColor, material_specular, material_shininess, material_metallic
		);
	}

	for (int i = 0; i < ubo.numSpotlights; i++) {
		Spotlight light = ubo.spotlights[i];	
		
		vec3 L = normalize(light.position.xyz - fragPosWorld);
		float theta = dot(L, normalize(-light.direction.xyz));
		float outerCosAngle =  cos(light.angle.x);
		if(theta > outerCosAngle) { 
			float epsilon  = cos(light.angle.y) - outerCosAngle;
			float intensity = clamp((theta - outerCosAngle) / epsilon, 0.0, 1.0); 
	
			lightOutput += 	intensity * calculateLight(fragColor, F0, N, V, 
				light.position.xyz, light.color, light.radius, 
				fragColor, material_specular, material_shininess, material_metallic
			); 
		}
	}
	
	for (int i = 0; i < ubo.numDirectionalLights; i++) {
		DirectionalLight directionalLight = ubo.directionalLights[i];
	
		float lightIntensity = max(dot(N, normalize(directionalLight.direction.xyz)), 0) ;
		vec3 color_intensity = directionalLight.color.xyz * directionalLight.color.w ;
	
		float Kd = 1.0;
		Kd *= 1.0 - material_metallic;
		lightOutput += lightIntensity * color_intensity * fragColor * Kd;
	}
#endif

	outColor = vec4(
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	ubo.ambientLightColor.xyz * ubo.ambientLightColor.w * 
#endif
	
	fragColor
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	+ lightOutput, 
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_TRANSLUCENT
	texture(tex_opacity, fragUV).x * factor.diffuse.w
#else
	1.0
#endif
	); //RGBA
}