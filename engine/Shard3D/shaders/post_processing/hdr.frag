#version 450
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D inputImage;

#define gamma 2.2
#define exposure 1.0


void main()	{
    vec3 hdrColor = texture(inputImage, fragUV).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);


    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
	outColor = vec4(mapped.xyz, 1.0);
}



