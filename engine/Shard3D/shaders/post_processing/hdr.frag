#version 450
#extension GL_KHR_vulkan_glsl: enable
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0, rgba32f) uniform image2D postProcessImage;

#define exposure 1.0

void main()	{
    vec3 hdrColor = imageLoad(postProcessImage, ivec2(gl_FragCoord.xy)).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    imageStore(postProcessImage, ivec2(gl_FragCoord.xy), vec4(mapped, 1.0));

	outColor = vec4(1.0);
}