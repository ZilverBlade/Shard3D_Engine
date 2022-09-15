#version 450
#extension GL_GOOGLE_include_directive : enable

#include "assets/_engine/shaderdata/surface_material/global_ubo.glsl"

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outPosition;
layout (location = 2) out vec4 outNormal;
layout (location = 3) out vec4 outMaterialData;

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
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED
layout(set = 2, binding = 6) uniform sampler2D tex_mask;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_TRANSLUCENT
layout(set = 2, binding = 7) uniform sampler2D tex_opacity;
#endif
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
layout(set = 2, binding = 8) uniform sampler2D tex_clearcoat;
#endif
layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float specularExponent(float dotNH, float shine, float sourceRadius){
	return pow(max(dotNH, 0) * (1 + sourceRadius / 100), shine * 256 + 2);
}

vec3 calculateLight(vec3 incolor, vec3 baseSpecular, vec3 normal, vec3 viewpoint, vec3 lightPos, vec4 l_Color, float l_Radius, vec3 m_Color, float m_Specular, float m_Shininess, float m_Metallic
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
	, float m_ClearCoat
#endif
) {
	vec3 L = normalize(lightPos - fragPosWorld);
	vec3 H = normalize(L + viewpoint);

	float dist = length(lightPos - fragPosWorld);
	float attenuation = 1.0 / (dist * dist);
	vec3 radiance = l_Color.xyz * l_Color.w * attenuation;
	
	float diffuseLight = max(dot(normal, L), 0);	
	
	vec3 diffuse = incolor * radiance * diffuseLight;

	float fresnelFactor = dot(normal, viewpoint); // Or dot(normal, eye).
		fresnelFactor = max(fresnelFactor, 0.1);

	vec3 fresnel = baseSpecular * pow(min(1.2 - fresnelFactor, 1.0), 4.0) * 5;

	float specularLight = specularExponent(dot(normal, H), fresnelFactor * m_Shininess, l_Radius);
	
	vec3 specular = m_Specular * radiance * specularLight;

	vec3 Kd = vec3(1.0);
	
	Kd *= 1.0 - m_Metallic;

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
// fake clearcoat fuckery
	float rimLightIntensity = dot(viewpoint, normal);
		rimLightIntensity = 1.0 - rimLightIntensity;
		rimLightIntensity = max(0.0, rimLightIntensity);

	float rimLight = pow(rimLightIntensity, 4.2) * 0.3;

#endif

	return Kd * diffuse + fresnel * specular
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
	+ m_ClearCoat * rimLight
#endif
	;
}
	
// shader code
void main(){
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED
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

#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
	const float material_clearcoat = texture(tex_clearcoat, fragUV).x * factor.clearcoat;
#endif	

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
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
	,material_clearcoat
#endif		
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
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_CLEARCOAT
	,material_clearcoat
#endif		
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

const vec3 calculatedLight = vec3(
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	ubo.ambientLightColor.xyz * ubo.ambientLightColor.w * 
#endif
	fragColor
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	+ lightOutput 
#endif
);

const float opacity = 
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_TRANSLUCENT
	texture(tex_opacity, fragUV).x * factor.diffuse.w
#else
	1.0
#endif
;
	outColor = vec4(calculatedLight, opacity); //RGBA
	outPosition = vec4(fragPosWorld, 1.0);
#ifdef S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED
	outNormal = vec4(N, 1.0); 
	outMaterialData = vec4(material_specular, material_shininess,material_metallic, 1.0); //RGBA
#endif
}