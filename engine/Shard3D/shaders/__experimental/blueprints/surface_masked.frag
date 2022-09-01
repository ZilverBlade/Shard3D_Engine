#define S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED

#define S3DSDEF_SURFACE_MASKED_UNIFORMSAMPLER2D_EXT layout(set = 2, binding = 6) uniform sampler2D tex_mask;

#define S3DSDEF_SURFACE_MASKEDFUNC if (texture(tex_mask, fragUV).x < 0.5) discard;
