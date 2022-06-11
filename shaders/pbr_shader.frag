#version 450

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
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

float PI = 3.1415926;

float roughness = 0.1f;
float metallic = 0.2f;
vec3 specularFactor = vec3(0.5);

float wrapDiffuse(vec3 normal, vec3 lightVector, float wrap) {
    return max(0.f, (dot(lightVector, normal) + wrap) / (1.f + wrap));
}

float getFogFactor(float d) {
	const float FogMax = 20.0;
	const float FogMin = 10.0;

	if (d>=FogMax) return 1;
	if (d<=FogMin) return 0;

	return 1 - (FogMax - d) / (FogMax - FogMin);
}

// phong (lambertian) diffuse term
float phong_diffuse() {
    return (1.0 / PI);
}

vec3 fresnel_factor(in vec3 f0, in float product) {
    return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
}
// Cook-torrance specular model
float D_blinn(in float roughness, in float NdH) {
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;
    return (n + 2.0) / (2.0 * PI) * pow(NdH, n);
}

float D_beckmann(in float roughness, in float NdH) {
    float m = roughness * roughness;
    float m2 = m * m;
    float NdH2 = NdH * NdH;
    return exp((NdH2 - 1.0) / (m2 * NdH2)) / (PI * m2 * NdH2 * NdH2);
}

float D_GGX(in float roughness, in float NdH) {
    float m = roughness * roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (PI * d * d);
}

float G_schlick(in float roughness, in float NdV, in float NdL) {
    float k = roughness * roughness * 0.5;
    float V = NdV * (1.0 - k) + k;
    float L = NdL * (1.0 - k) + k;
    return 0.25 / (V * L);
}
vec3 cooktorrance_specular(in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness) {
    //float D = D_blinn(roughness, NdH);

    float D = D_beckmann(roughness, NdH);

	//float D = D_GGX(roughness, NdH);

    float G = G_schlick(roughness, NdV, NdL);

    float rim = mix(1.0 - roughness * 0.1 * 0.9, 1.0, NdV);

    return (1.0 / rim) * specular * G * D;
}


// shader code
void main(){

	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

	vec3 specularLight = vec3(0.0);
	
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

	//Pointlight
	for (int i = 0; i < ubo.numPointlights; i++) {
		Pointlight pointlight = ubo.pointlights[i];

		vec3 lightDistance = pointlight.position.xyz - fragPosWorld;

		float attenuation = 1.0 / (
	/*				c		*/		pointlight.attenuationMod.x +																
	/*				bx		*/		pointlight.attenuationMod.y * length(lightDistance) +  
	/*				ax^2	*/		pointlight.attenuationMod.z * dot(lightDistance, lightDistance)) ;							
		lightDistance = normalize(lightDistance);
		
		float cosAngIndicence = max(dot(surfaceNormal, normalize(lightDistance)), 0);
		vec3 color_intensity = pointlight.color.xyz * pointlight.color.w * attenuation;

	 vec3 specular = mix(vec3(0.04), fragColor, metallic);

    vec3 F = normalize(-fragPosWorld);
	vec3 H = normalize(lightDistance + F);
	vec3 N = normalize(fragNormalWorld);

	float NdL = max(0.0, dot(N, lightDistance));
    float NdV = max(0.001, dot(N, F));
    float NdH = max(0.001, dot(N, H));
    float HdV = max(0.001, dot(H, F));

	vec3 specfresnel = fresnel_factor(specular, HdV);

    vec3 specref = cooktorrance_specular(NdL, NdV, NdH, specfresnel, roughness);

	vec3 diffref = (vec3(1.0) - specfresnel) * phong_diffuse() * NdL;

	specularLight += specref * color_intensity * pointlight.specularMod;
	diffuseLight += diffref * color_intensity;

	}

	// Spotlight
	for (int i = 0; i < ubo.numSpotlights; i++) {
		Spotlight spotlight = ubo.spotlights[i];

		vec3 lightDistance = spotlight.position.xyz - fragPosWorld;

		float attenuation = 1.0 / (
	/*				c		*/		spotlight.attenuationMod.x +																
	/*				bx		*/		spotlight.attenuationMod.y * length(lightDistance) +  
	/*				ax^2	*/		spotlight.attenuationMod.z * dot(lightDistance, lightDistance)) ;							
		lightDistance = normalize(lightDistance);
		float cosAngIndicence = max(dot(surfaceNormal, normalize(lightDistance)), 0);
		vec3 color_intensity = spotlight.color.xyz * spotlight.color.w * attenuation;

		float theta = dot(lightDistance, normalize(-spotlight.direction.xyz));
		float epsilon  = spotlight.angle.y - spotlight.angle.x;
		float intensity = clamp((theta - spotlight.angle.x) / epsilon, 0.0, 1.0); 

		if(theta > sin(spotlight.angle.x)) { 

				 vec3 specular = mix(vec3(0.04), fragColor, metallic);

    vec3 F = normalize(-fragPosWorld);
	vec3 H = normalize(lightDistance + F);
	vec3 N = normalize(fragNormalWorld);

	float NdL = max(0.0, dot(N, lightDistance));
    float NdV = max(0.001, dot(N, F));
    float NdH = max(0.001, dot(N, H));
    float HdV = max(0.001, dot(H, F));

			vec3 specfresnel = fresnel_factor(specular, HdV);

			vec3 specref = cooktorrance_specular(NdL, NdV, NdH, specfresnel, roughness);

			vec3 diffref = (vec3(1.0) - specfresnel) * phong_diffuse() * NdL;

			specularLight += specref * color_intensity * spotlight.specularMod;
			diffuseLight += diffref * color_intensity;

		}		
	}

	// Directional light
	for (int i = 0; i < ubo.numDirectionalLights; i++) {
		DirectionalLight directionalLight = ubo.directionalLights[i];

		float lightIntensity = max(dot(surfaceNormal, normalize(directionalLight.direction.xyz)), 0) * wrapDiffuse(surfaceNormal, normalize(directionalLight.direction.xyz), 1.f) ;
		vec3 color_intensity = directionalLight.color.xyz * directionalLight.color.w ;

		//basic directional light shader
		diffuseLight += lightIntensity * color_intensity;
		
	}

	vec3 fogColor =  vec3(0.3f, 0.3f, 0.8f);
		// multiply fragColor by specular only if material is metallic
	outColor = vec4(mix(diffuseLight * mix(fragColor, vec3(0.0), vec3(0.00)) + specularLight, fogColor, getFogFactor(distance(cameraPosWorld, fragPosWorld))), 1.0); //RGBA
	//outColor = vec4(diffuseLight * fragColor + specularLight, 1.0); //RGBA
}