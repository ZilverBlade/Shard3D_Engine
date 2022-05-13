#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormal;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projectionViewMatrix;

	vec4 ambientLightColor;			//	sky/ambient
	vec3 directionalLightDirection;	//	directional
	
	vec3 pointlightPosition;		//	point
	vec4 pointlightColor;			//	point
	vec3 pointlightAttenuationMod;  //	const + linear * x + quadratic * x^2

} ubo;


layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

void main(){
	vec3 lightDistance = ubo.pointlightPosition - fragPosWorld;
	float attenuation = ubo.pointlightColor.w / 
	/*						c		*/			(ubo.pointlightAttenuationMod.x +																
	/*						bx		*/			ubo.pointlightAttenuationMod.y * sqrt(lightDistance.x*lightDistance.x + lightDistance.y*lightDistance.y + lightDistance.z*lightDistance.z) +  
	/*						ax^2	*/			ubo.pointlightAttenuationMod.z * dot(lightDistance, lightDistance));							

	vec3 pointlightColor = ubo.pointlightColor.xyz * ubo.pointlightColor.w * attenuation;
	vec3 diffuseLight = pointlightColor * max(dot(normalize(fragNormal), normalize(lightDistance)), 0);

//	float lightIntensity = ubo.ambientLightColor.w + max(dot(normalize(fragNormal), ubo.directionalLightDirection), 0);

	outColor = vec4((diffuseLight +  ubo.ambientLightColor.xyz * ubo.ambientLightColor.w) * fragColor, 1.0); //RGBA
}