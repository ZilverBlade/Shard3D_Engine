// Ray origin, ray destination
// https://timcoster.com/2020/02/11/raymarching-shader-pt1-glsl/
// https://lettier.github.io/3d-game-shaders-for-beginners/screen-space-reflection.html

struct Ray {
	int max_steps;
	float max_dist;
	float resolution;
    float thickness;
    vec3 o; // ray origin
    vec3 d; // ray destination
}

float RayMarch(Ray ray) 
{
    float dO = 0.0; //Distane Origin
    for(int i = 0 ; i < ray.max_steps ; i++)
    {
        vec3 p = ray.o + ray.d * dO;
        float ds = GetDist(p); // ds is Distance Scene
        dO += ds;
        if (vec(ray.currPos) < 0 || maxVec3(ray.currPos) > 1)
            return vec4(0.0);
    }
    return dO;
}
 