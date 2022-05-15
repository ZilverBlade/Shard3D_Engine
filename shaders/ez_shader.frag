#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;


struct Pointlight {
	vec4 position;
	vec4 color;
	vec4 attenuationMod; //	const + linear * x + quadratic * x^2
};

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;

	vec4 ambientLightColor;			//	sky/ambient
	//vec3 directionalLightDirection;	//	directional
	
	Pointlight pointlights[20];
	int numLights;

} ubo;


layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

void main(){
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight = vec3(0.0);

	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

	for (int i = 0; i < ubo.numLights; i++) {
		Pointlight pointlight = ubo.pointlights[i];

		vec3 lightDistance = pointlight.position.xyz - fragPosWorld;
		float attenuation = 1.0 / (
	/*				c		*/		pointlight.attenuationMod.x +																
	/*				bx		*/		pointlight.attenuationMod.y * sqrt(lightDistance.x*lightDistance.x + lightDistance.y*lightDistance.y + lightDistance.z*lightDistance.z) +  
	/*				ax^2	*/		pointlight.attenuationMod.z * dot(lightDistance, lightDistance)) ;							
		lightDistance = normalize(lightDistance);
		
		float cosAngIndicence = max(dot(surfaceNormal, normalize(lightDistance)), 0);
		vec3 color_intensity = pointlight.color.xyz * pointlight.color.w * attenuation;

		diffuseLight += color_intensity * cosAngIndicence;

		//specular lightDistance
		vec3 halfAngle = normalize(lightDistance + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 128.0); //higher val -> sharper light (16 - 1024)
		specularLight += color_intensity * blinnTerm; 
	}

//	float lightIntensity = ubo.ambientLightColor.w + max(dot(normalize(fragNormalWorld), ubo.directionalLightDirection), 0);w

		// multiply fragColor by specular only if material is metallic
	outColor = vec4(diffuseLight * fragColor + specularLight, 1.0); //RGBA
}