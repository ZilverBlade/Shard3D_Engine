#version 450
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D inputImage;

#define gamma 2.2

void main()	{   
    // gamma correction 
    vec3 corrected = pow(texture(inputImage, fragUV).xyz, vec3(1.0 / gamma));

	outColor = vec4(corrected.xyz, 1.0);
}