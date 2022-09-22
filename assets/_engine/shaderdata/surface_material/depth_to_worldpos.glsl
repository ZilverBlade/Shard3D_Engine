// this is supposed to get the world position from the depth buffer
vec3 WorldPosFromDepth(vec2 fragCoord, vec2 screenArea, float depth, mat4 invProjectMatrix, mat4 invViewMatrix) {
    vec2 localCoords = fragCoord / screenArea;
	
    vec4 clipSpacePosition = vec4(localCoords * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpacePosition = invProjectMatrix * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = invViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}