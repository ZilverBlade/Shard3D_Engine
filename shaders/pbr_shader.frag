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

float roughness = 0.3;
float metallic = 0.2;
float specular = 0.5;
vec3 baseRefl = vec3(0.0);

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

// phong (lambertian) diffuse term
float lambertian_diffuse() {
    return (1.0 / PI);
}

vec3 F_schlick(in vec3 f0, in float NdH) {
    return mix(f0, vec3(1.0), pow(1.01 - NdH, 5.0));
}

//GGX Trowbridge-Reitz
float D_GGX(in float r, in float NdH) {
    float alpha = r * r;
    float alpha2 = alpha * alpha;
    float d = (NdH * NdH) * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * d * d);
}
// Slick-GGX
float G_schlick(in float r, in float NdV) {
    float k = r * 0.25;
	return NdV / (NdV * (1.0 - k) + k);
}
vec3 cooktorrance_specular(in float NdL, in float NdV, in float NdH, in vec3 s, in float r) {
	float D = D_GGX(r, NdH);
    float G = G_schlick(r, NdV);
	vec3 F = F_schlick(baseRefl, NdV);

	float d = max(4.0 * NdV * NdL, 0.00001);
	
    return ((D * G * F) / d) * s * 2; 
}

// shader code
void main(){

	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

	vec3 specularLight = vec3(0.0);
	
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);
	
	vec3 F = normalize(-fragPosWorld);
	vec3 N = normalize(fragNormalWorld);
	vec3 H = normalize(viewDirection + F);
	float NdF = max(0.001, dot(N, F));
	float NdV = max(0.001, dot(N, viewDirection));
	float NdH = max(0.001, dot(N, H));

	float HdV = max(0.001, dot(H, viewDirection));
	vec3 specfresnel = F_schlick(baseRefl, HdV);

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

		float NdL = max(0.001, dot(N, lightDistance));	
		vec3 specref = cooktorrance_specular(NdL, NdV, NdH, specfresnel, roughness);

		vec3 diffref = (vec3(1.0) - specfresnel) * lambertian_diffuse() * NdL;

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

			float NdL = max(0.001, dot(N, lightDistance));
	
			vec3 specref = cooktorrance_specular(NdL, NdV, NdH, specfresnel, roughness);

			vec3 diffref = (vec3(1.0) - specfresnel) * lambertian_diffuse() * NdL;

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
	vec3 BRDF = diffuseLight * fragColor + specularLight;
	outColor = vec4(mix(BRDF, fogColor, getFogFactor(distance(cameraPosWorld, fragPosWorld))), 1.0); //RGBA
	//outColor = vec4(diffuseLight * fragColor + specularLight, 1.0); //RGBA
}