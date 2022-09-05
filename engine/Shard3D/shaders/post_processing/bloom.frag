#version 450
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D inputRenderedScene;
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

void main()	{
    vec2 tex_offset = 1.0 / textureSize(inputRenderedScene, 0); // gets size of single texel
   
    vec3 bloomHorizontal = texture(inputRenderedScene, fragUV).xyz * weight[0]; //= blur9(inputImage, fragUV, vec2(1920.0, 1080.0), vec2(1.0, 0.0));
	vec3 bloomVertical =  texture(inputRenderedScene, fragUV).xyz * weight[0];//= blur9(inputImage, fragUV, vec2(1920.0, 1080.0), vec2(0.0, 1.0));

	for(int i = 1; i < 6; ++i)
    {
        bloomHorizontal += texture(inputRenderedScene, fragUV + vec2(tex_offset.x * i, 0.0)).xyz * weight[i];
        bloomHorizontal += texture(inputRenderedScene, fragUV - vec2(tex_offset.x * i, 0.0)).xyz * weight[i];
    }
    for(int i = 1; i < 6; ++i)
    {
        bloomVertical += texture(inputRenderedScene, fragUV + vec2(0.0, tex_offset.y * i)).xyz * weight[i];
        bloomVertical += texture(inputRenderedScene, fragUV - vec2(0.0, tex_offset.y * i)).xyz * weight[i];
    }

   vec3 bloomTerm = max((bloomHorizontal + bloomVertical) - vec3(bloomThreshold * 2.0), vec3(0.0));

	outColor = vec4(texture(inputRenderedScene, fragUV).xyz + bloomTerm, 1.0);
}



