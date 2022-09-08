#version 450
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;
layout (set = 0, binding = 0, rgba32f) uniform image2D postProcessImage;

const highp float NOISE_GRANULARITY = 0.3/1023.0;

highp float random(vec2 coords) {
   return fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main()	{
    vec2 coordinates = gl_FragCoord.xy / vec2(imageSize(postProcessImage));
    
    vec3 pixelColor = imageLoad(postProcessImage, ivec2(gl_FragCoord.xy)).rgb;
    
    float fragmentColor_r = pixelColor.x;
    float fragmentColor_g = pixelColor.y;
    float fragmentColor_b = pixelColor.z;
    fragmentColor_r += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, random(coordinates));
	fragmentColor_g += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, random(coordinates));
	fragmentColor_b += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, random(coordinates));

     outColor = vec4(fragmentColor_r, fragmentColor_g, fragmentColor_b, 1.0);
}