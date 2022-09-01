#define S3DSDEF_SHADER_PERMUTATION_SURFACE_TRANSLUCENT
														// binding 6 is reserved for masked
#define S3DSDEF_SURFACE_TRANSLUCENT_UNIFORMSAMPLER2D_EXT layout(set = 2, binding = 7) uniform sampler2D tex_opacity;
		
#define S3DSDEF_SURFACE_ALPHAFUNC texture(tex_opacity, fragUV).x * factor.diffuse.w