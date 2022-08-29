#version 450
layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D inputImage;


    const vec2 res = vec2(1920, 1080);
 
	const float Pi = 6.28318530718; // Pi*2
    
    // GAUSSIAN BLUR SETTINGS {{{
    const float Directions = 8.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    const float Quality = 8.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    const float Size = 4.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}

void main()	{
    const vec2 pos = gl_FragCoord.xy;

    vec3 pixel = texture(inputImage, fragUV).xyz;
	//float bloomStrength = bloomMap.x + bloomMap.y + bloomMap.z;


   
    vec2 Radius = Size/res.xy;
    
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = pos/res.xy;
    // Pixel colour
    vec4 Color = vec4(0.0);
    
    if (pixel.x > 0.0 || pixel.y > 0.0 || pixel.z > 0.0 )
    // Blur calculations
    for( float d=0.0; d<Pi; d+=Pi/Directions)   {
		for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)  {
			Color += max(texture(inputImage, uv+vec2(cos(d),sin(d))*Radius*i)- vec4(1.0), vec4(0.0));		
        }
    }


	outColor = vec4(texture(inputImage, fragUV).xyz, 1.0) + Color;
}