#version 450
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D postProcessImage;

const highp float NOISE_GRANULARITY = 0.3/255.0;

highp float random(vec2 coords) {
   return fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);
}

vec3 debander(vec3 pixelColor) {
    vec2 coordinates = gl_FragCoord.xy / vec2(textureSize(postProcessImage, 0));
   
    float fragmentColor_r = pixelColor.x;
    float fragmentColor_g = pixelColor.y;
    float fragmentColor_b = pixelColor.z;
    fragmentColor_r += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, random(coordinates));
	fragmentColor_g += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, random(coordinates));
	fragmentColor_b += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, random(coordinates));
    return vec3(fragmentColor_r, fragmentColor_g, fragmentColor_b);
}


#define bloomThreshold 1.0
#define bloomStrength 5

//https://github.com/Jam3/glsl-fast-gaussian-blur

const float weight[6] = float[] (0.252135, 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3846153846) * direction;
  vec2 off2 = vec2(3.2307692308) * direction;
  color += texture(image, uv) * 0.2270270270;
  color += texture(image, uv + (off1 / resolution)) * 0.3162162162;
  color += texture(image, uv - (off1 / resolution)) * 0.3162162162;
  color += texture(image, uv + (off2 / resolution)) * 0.0702702703;
  color += texture(image, uv - (off2 / resolution)) * 0.0702702703;
  return color;
}

vec3 bloom(vec3 pixelColor) {
    vec2 tex_offset = 1.0 / textureSize(postProcessImage, 0); // gets size of single texel
   
    vec3 bloomHorizontal = max(pixelColor - 1.0, 0.0) * weight[0]; //= blur9(inputImage, fragUV, vec2(1920.0, 1080.0), vec2(1.0, 0.0));
	vec3 bloomVertical = bloomHorizontal;//= blur9(inputImage, fragUV, vec2(1920.0, 1080.0), vec2(0.0, 1.0));

	for(int i = 1; i < 6; ++i)
    {
        bloomHorizontal += max(texture(postProcessImage, fragUV + vec2(tex_offset.x * i, 0.0)).xyz - 1.0, 0.0) * weight[i];
        bloomHorizontal += max(texture(postProcessImage, fragUV - vec2(tex_offset.x * i, 0.0)).xyz - 1.0, 0.0) * weight[i];
    }
    for(int i = 1; i < 6; ++i)
    {
        bloomVertical += max(texture(postProcessImage, fragUV + vec2(0.0, tex_offset.y * i)).xyz - 1.0, 0.0) * weight[i];
        bloomVertical += max(texture(postProcessImage, fragUV - vec2(0.0, tex_offset.y * i)).xyz - 1.0, 0.0) * weight[i];
    }

    vec3 bloomTerm = bloomHorizontal + bloomVertical;
    return bloomTerm;
}

void main()	{
    vec3 pixelColor = texture(postProcessImage, fragUV).rgb;

    pixelColor += bloom(pixelColor);

    outColor = vec4(debander(pixelColor), 1.0);
}