#version 450
#extension GL_ARB_shading_language_420pack : enable
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

struct Pointlight {
	vec4 position;
	vec4 color;
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
	float specularMod;
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
	
	Pointlight pointlights[20];
	Spotlight spotlights[20];
	DirectionalLight directionalLights[6];
	int numPointlights;
	int numSpotlights;
	int numDirectionalLights;

    vec3 materialSettings;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

float PI = 3.1415926;

vec3 F0 = vec3(0.0); // base reflectiveness

float wrapDiffuse(vec3 normal, vec3 lightVector, float wrap) {
    return max(0.f, (dot(lightVector, normal) + wrap) / (1.0 + wrap));
}

float getFogFactor(float d) {
	const float FogMax = 20.0;
	const float FogMin = 10.0;

	if (d>=FogMax) return 1;
	if (d<=FogMin) return 0;

	return 1 - (FogMax - d) / (FogMax - FogMin);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    //float r = (roughness + 1.0);
    float k = (roughness*roughness) / 2;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom + 0.01;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------



// shader code
void main(){
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

	vec3 specularLight = vec3(0.0);
	
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);
	vec3 N = fragNormalWorld;

    F0 = mix(F0, fragColor, ubo.materialSettings.z);


     vec3 V = normalize(cameraPosWorld - fragPosWorld);
     vec3 specular = vec3(1.f);
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < ubo.numPointlights; i++) {
        Pointlight light = ubo.pointlights[i];
        vec3 L = normalize(light.position.xyz - fragPosWorld);
        vec3 H = normalize(V + L);
        float distance = length(light.position.xyz - fragPosWorld);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light.color.xyz * light.color.w * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, clamp(ubo.materialSettings.y, 0.05, 1.0));
        float G   = GeometrySmith(N, V, L, clamp(ubo.materialSettings.y, 0.05, 1.0));
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0) * (1.0 - clamp(ubo.materialSettings.z, 0.0, 0.9));

        vec3 numerator    = NDF * G * F;
        float denominator = max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        specular = 2 * (numerator / denominator) * ubo.materialSettings.x;

        // kS is equal to Fresnel
       // vec3 kS = F;

        vec3 kD = vec3(1.0);// - kS;
       
        kD *= 1.0 - ubo.materialSettings.z;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * fragColor / PI + specular) * radiance * NdotL * 100;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    for (int i = 0; i < ubo.numSpotlights; i++) {
        Spotlight light = ubo.spotlights[i];
        
        
        vec3 L = normalize(light.position.xyz - fragPosWorld);
		float theta = dot(L, normalize(-light.direction.xyz));
		float outerCosAngle =  cos(light.angle.x);
		if(theta > outerCosAngle) { 
            float epsilon  = cos(light.angle.y) - outerCosAngle;
		    float intensity = clamp((theta - outerCosAngle) / epsilon, 0.0, 1.0); 

            vec3 H = normalize(V + L);
            float distance = length(light.position.xyz - fragPosWorld);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = light.color.xyz * light.color.w * attenuation;

            // Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, clamp(ubo.materialSettings.y, 0.05, 1.0));
            float G   = GeometrySmith(N, V, L, clamp(ubo.materialSettings.y, 0.05, 1.0));
            vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0) * (1.0 - clamp(ubo.materialSettings.z, 0.0, 0.9));

            vec3 numerator    = NDF * G * F;
            float denominator = 2 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
            specular = (numerator / denominator) * ubo.materialSettings.x;

            // kS is equal to Fresnel
            // vec3 kS = F;

            vec3 kD = vec3(1.0);// - kS;
       
            kD *= 1.0 - ubo.materialSettings.z;

            // scale light by NdotL
            float NdotL = max(dot(N, L), 0.0);

            // add to outgoing radiance Lo
            Lo += (kD * fragColor / PI + specular) * radiance * NdotL * 100 * intensity;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        }
    }
    
	vec3 fogColor = vec3(0.01, 0.01, 0.01);
	    outColor = vec4(mix((ubo.ambientLightColor.xyz + Lo) 
        * ubo.ambientLightColor.w, fogColor, getFogFactor(distance(cameraPosWorld, fragPosWorld))), 
        1.0); //RGBA
}