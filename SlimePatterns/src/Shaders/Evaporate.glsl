#version 460 core
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D Texture;

layout(std140, binding = 1) buffer Parameters
{
    vec4 ActiveSpeciesMask;
    int RandSeed;
    float TimeStep;
    int Width;
    int Height;
    //Pheromons params
    int KernelSize;
    float EvaporationFactor;
    float DifiusonFactor;
};

//Mainfunction
void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    vec4 orginal = imageLoad(Texture, id);
    orginal = max(vec4(0), orginal - orginal * EvaporationFactor * TimeStep);
    //orginal = max(vec4(0), orginal * (1.f-EvaporationFactor) * TimeStep);

    imageStore(Texture, id, orginal);
}