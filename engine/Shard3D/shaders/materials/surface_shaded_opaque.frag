#version 450

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

struct Pointlight {
	vec4 position;
	vec4 color;
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
	float specularMod;
	float radius;
};
struct Spotlight {
	vec4 position;
	vec4 color;
	vec4 direction; // (ignore w)
	vec2 angle; //outer, inner
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
	float specularMod;
};
struct DirectionalLight {
	vec4 position;
	vec4 color;
	vec4 direction; //	directional (ignore w)
	float specularMod;	
};

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;

	vec4 ambientLightColor;			//	sky/ambient
	
	Pointlight pointlights[128];
	Spotlight spotlights[128];
	DirectionalLight directionalLights[6];
	int numPointlights;
	int numSpotlights;
	int numDirectionalLights;
} ubo;

layout(set = 1, binding = 1) uniform vec4 diffuse_factor;
layout(set = 1, binding = 2) uniform float specular_factor;
layout(set = 1, binding = 3) uniform float shininess_factor;
layout(set = 1, binding = 4) uniform float metallic_factor;

layout(set = 2, binding = 1) uniform sampler2D diffuse_tex;
layout(set = 2, binding = 2) uniform sampler2D specular_tex;
layout(set = 2, binding = 3) uniform sampler2D shininess_tex;
layout(set = 2, binding = 4) uniform sampler2D metallic_tex;
layout(set = 2, binding = 5) uniform sampler2D normal_tex;


layout(set = 1, binding = 1) uniform MaterialTex {
	mat4 projection;
	mat4 view;
	mat4 invView;

	vec4 ambientLightColor;			//	sky/ambient
	
	Pointlight pointlights[128];
	Spotlight spotlights[128];
	DirectionalLight directionalLights[6];
	int numPointlights;
	int numSpotlights;
	int numDirectionalLights;

	vec3 materialSettings;
} material_tex;

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
 return pow(max(dotNH, 0) * (1 + sourceRadius / 100), shine * 512 + 4) * PGS;
}
#define LIGHT_RADIUS 1

// shader code
void main(){
	float material_specular = texture(specular_tex, fragUV).x * specular_factor;
	float material_shininess = texture(shininess_tex, fragUV).x * shininess_factor;
	float material_metallic = texture(metallic_tex, fragUV).x * metallic_factor;
	vec3 fragColor = texture(diffuse_tex, fragUV).xyz * diffuse_factor.xyz;

	vec3 F0 = vec3(1.0); // base specular
	F0 = mix(F0, fragColor, material_metallic);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	
	vec3 N = normalize(fragNormalWorld);
	vec3 V = normalize(cameraPosWorld - fragPosWorld);
	
	vec3 lightOutput = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	
	for (int i = 0; i < ubo.numPointlights; i++) {
		Pointlight light = ubo.pointlights[i];
		vec3 L = normalize(light.position.xyz - fragPosWorld);
		vec3 H = normalize(L + V);

		float dist = length(light.position.xyz - fragPosWorld);
		float attenuation = 1.0 / (dist * dist);
		vec3 radiance = light.color.xyz * light.color.w * attenuation;
		
		float diffuseLight = max(dot(N, L), 0);	
		
		vec3 diffuse = fragColor * radiance * diffuseLight;

		float G   = PseudoGeometrySmith(N, V, material_shininess);
		vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
		float specularLight = specularExponent(dot(N, H), material_shininess, G, light.radius);

		vec3 specular = material_specular * radiance * specularLight;

		vec3 Kd = vec3(1.0);
	   
		Kd *= 1.0 - material_metallic;

		lightOutput += Kd * diffuse + specular * max(F, 0); 
	}

	for (int i = 0; i < ubo.numSpotlights; i++) {
		Spotlight light = ubo.spotlights[i];	
		
		vec3 L = normalize(light.position.xyz - fragPosWorld);
		float theta = dot(L, normalize(-light.direction.xyz));
		float outerCosAngle =  cos(light.angle.x);
		if(theta > outerCosAngle) { 
			float epsilon  = cos(light.angle.y) - outerCosAngle;
			float intensity = clamp((theta - outerCosAngle) / epsilon, 0.0, 1.0); 
	
			vec3 H = normalize(L + V);
			float dist = length(light.position.xyz - fragPosWorld);
			float attenuation = 1.0 / (dist * dist);
			vec3 radiance = light.color.xyz * light.color.w * attenuation;
			
			float diffuseLight = max(dot(N, L), 0);	
		
			vec3 diffuse = fragColor * radiance * diffuseLight;

			float G   = PseudoGeometrySmith(N, V, material_shininess);
			vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
			float specularLight = specularExponent(dot(N, H), material_shininess, G, LIGHT_RADIUS);

			vec3 specular = material_specular * radiance * specularLight;

			vec3 Kd = vec3(1.0);
	   
			Kd *= 1.0 - material_metallic;

			lightOutput += (Kd * diffuse  + specular * max(F, 0)) * intensity; 
		}
	}
	
	for (int i = 0; i < ubo.numDirectionalLights; i++) {
		DirectionalLight directionalLight = ubo.directionalLights[i];
	
		float lightIntensity = max(dot(N, normalize(directionalLight.direction.xyz)), 0) ;
		vec3 color_intensity = directionalLight.color.xyz * directionalLight.color.w ;
	
		lightOutput += lightIntensity * color_intensity;
	}

	outColor = vec4(lightOutput, 1.0); //RGBA
}