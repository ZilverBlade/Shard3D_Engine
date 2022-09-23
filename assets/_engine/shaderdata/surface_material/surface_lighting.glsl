
#include "global_ubo.glsl"

struct ShadedPixelInfo {
	vec3 diffuse;
	vec3 positionWorld;
	vec3 normal;
	float opacity;
	float specular;
	float shininess;
	float metallic;
};

float specularExponent(float dotNH, float shine, float sourceRadius) {
	return pow(max(dotNH, 0) * (1 + sourceRadius / 100), shine * 256 + 2);
}

vec3 calculateLightFromSource(vec3 baseSpecular, vec3 viewpoint, vec3 radiance, vec3 l_Pos, float l_Radius, ShadedPixelInfo pixel) {
	vec3 L = normalize(l_Pos - pixel.positionWorld);
	vec3 H = normalize(L + viewpoint);

	float diffuseLight = max(dot(pixel.normal, L), 0);

	vec3 diffuse = pixel.diffuse * radiance * diffuseLight;

	float fresnelFactor = max(dot(pixel.normal, viewpoint), 0.1);

	vec3 fresnel = baseSpecular * pow(min(1.2 - fresnelFactor, 1.0), 4.0) * 5;

	float specularLight = specularExponent(dot(pixel.normal, H), fresnelFactor * pixel.shininess, l_Radius);

	vec3 specular = pixel.specular * radiance * specularLight;

	vec3 Kd = vec3(1.0);

	Kd *= 1.0 - pixel.metallic;

#if 0 
	// fake clearcoat fuckery
	float rimLightIntensity = dot(viewpoint, normal);
	rimLightIntensity = 1.0 - rimLightIntensity;
	rimLightIntensity = max(0.0, rimLightIntensity);

	float rimLight = pow(rimLightIntensity, 4.2) * 0.3;

#endif

	return Kd * diffuse + fresnel * specular;
}


vec3 calculateLight(ShadedPixelInfo pixel) {
	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 V = normalize(cameraPosWorld - pixel.positionWorld);

	vec3 lightOutput = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w * pixel.diffuse;

	vec3 F0 = vec3(1.0); // base specular
	F0 = mix(F0, pixel.diffuse, pixel.metallic);

	for (int i = 0; i < ubo.numPointlights; i++) {
		Pointlight light = ubo.pointlights[i];
		float dist = length(light.position.xyz - pixel.positionWorld);
		float attenuation = 1.0 / (dist * dist);
		
		float strength = light.color.w * attenuation;
		if (strength < 0.001) continue;	// light culling
		vec3 radiance = strength * light.color.xyz;

		lightOutput +=
			calculateLightFromSource(F0, V,
				radiance, light.position.xyz, light.radius, pixel);
	}

	for (int i = 0; i < ubo.numSpotlights; i++) {
		Spotlight light = ubo.spotlights[i];

		float dist = length(light.position.xyz - pixel.positionWorld);
		float attenuation = 1.0 / (dist * dist);
		
		float strength = light.color.w * attenuation;
		if (strength < 0.001) continue;	// light culling
		vec3 radiance = strength * light.color.xyz;

		vec3 L = normalize(light.position.xyz - pixel.positionWorld);
		float theta = dot(L, normalize(-light.direction.xyz));
		float outerCosAngle = cos(light.angle.x);
		if (theta > outerCosAngle) {
			float epsilon = cos(light.angle.y) - outerCosAngle;
			float intensity = clamp((theta - outerCosAngle) / epsilon, 0.0, 1.0);

			lightOutput +=
				calculateLightFromSource(F0, V,
					radiance, light.position.xyz, light.radius, pixel) * intensity;
		}
	}

	for (int i = 0; i < ubo.numDirectionalLights; i++) {
		DirectionalLight directionalLight = ubo.directionalLights[i];

		float lightIntensity = max(dot(pixel.normal, normalize(directionalLight.direction.xyz)), 0);
		vec3 color_intensity = directionalLight.color.xyz * directionalLight.color.w;

		float Kd = 1.0;
		Kd *= 1.0 - pixel.metallic;
		lightOutput += lightIntensity * color_intensity * pixel.diffuse * Kd;
	}

	return lightOutput;
}
