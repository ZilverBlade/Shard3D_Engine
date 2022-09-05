#version 450 core
layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
void main()
{
    fColor = vec4(pow(In.Color.xyz * texture(sTexture, In.UV.st).xyz, vec3(0.454545)), In.Color.w * texture(sTexture, In.UV.st).w);
}