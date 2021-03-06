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

	vec3 materialSettings;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

float wrapDiffuse(vec3 normal, vec3 lightVector, float wrap) {
    return max(0.f, (dot(lightVector, normal) + wrap) / (1.f + wrap));
}

float getFogFactor(float d) {
	const float FogMax = 100.0;
	const float FogMin = 30.0;

	if (d>=FogMax) return 1;
	if (d<=FogMin) return 0;

	return 1 - (FogMax - d) / (FogMax - FogMin);
}

void main() {
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

	vec3 specularLight = vec3(0.0);
	float blinnPow = max((1 - ubo.materialSettings.y) * 4096.0, 16.0);//higher val -> sharper light (16 - 1024)

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

		diffuseLight += color_intensity * cosAngIndicence;

			// specular
		vec3 halfAngle = normalize(lightDistance + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, blinnPow); 
		specularLight += color_intensity * blinnTerm * pointlight.specularMod * ubo.materialSettings.x; 
	}

	// Spotlight
	for (int i = 0; i < ubo.numSpotlights; i++) {
		Spotlight spotlight = ubo.spotlights[i];

		vec3 lightDistance = spotlight.position.xyz - fragPosWorld;

		//attenuation mod is broken for spotlights
		float attenuation = 1.0 / dot(lightDistance, lightDistance); //(
	/*				c		*/		//spotlight.attenuationMod.x +																
	/*				bx		*/		//spotlight.attenuationMod.y * length(lightDistance) +  
	/*				ax^2	*/		//spotlight.attenuationMod.z * dot(lightDistance, lightDistance)) ;							
		lightDistance = normalize(lightDistance);
		float cosAngIndicence = max(dot(surfaceNormal, normalize(lightDistance)), 0);
		vec3 color_intensity = spotlight.color.xyz * spotlight.color.w * attenuation;

        vec3 L = normalize(spotlight.position.xyz - fragPosWorld);
		float theta = dot(L, normalize(-spotlight.direction.xyz));
		float outerCosAngle =  cos(spotlight.angle.x);
		if(theta > outerCosAngle) { 
            float epsilon  = cos(spotlight.angle.y) - outerCosAngle;
		    float intensity = clamp((theta - outerCosAngle) / epsilon, 0.0, 1.0); 
			diffuseLight += color_intensity * cosAngIndicence * intensity ;
				//specular 
			vec3 halfAngle = normalize(lightDistance + viewDirection);
			float blinnTerm = dot(surfaceNormal, halfAngle);
			blinnTerm = clamp(blinnTerm, 0, 1);
			blinnTerm = pow(blinnTerm, blinnPow); 
			specularLight += color_intensity * blinnTerm * intensity * spotlight.specularMod * ubo.materialSettings.x; 
		}		
	}

	// Directional light
	for (int i = 0; i < ubo.numDirectionalLights; i++) {
		DirectionalLight directionalLight = ubo.directionalLights[i];

		float lightIntensity = max(dot(surfaceNormal, normalize(directionalLight.direction.xyz)), 0) * wrapDiffuse(surfaceNormal, normalize(directionalLight.direction.xyz), 1.f) ;
		vec3 color_intensity = directionalLight.color.xyz * directionalLight.color.w ;

		diffuseLight += lightIntensity * color_intensity;
		
		if (directionalLight.specularMod != 0.f){
				// specular 
		vec3 halfAngle = normalize(directionalLight.position.xyz + vec3(0, 1000.f, 0) - fragPosWorld + viewDirection) * 0.999f;
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, blinnPow); 
		specularLight += color_intensity * blinnTerm * directionalLight.specularMod * ubo.materialSettings.x; 
		}
	}

	vec3 fogColor =  vec3(0.01, 0.01, 0.01);
		// multiply fragColor by specular only if material is metallic
	outColor = vec4(mix(diffuseLight * fragColor + specularLight, fogColor, getFogFactor(distance(cameraPosWorld, fragPosWorld))), 1.0); //RGBA
	//outColor = vec4(diffuseLight * fragColor + specularLight, 1.0); //RGBA
}