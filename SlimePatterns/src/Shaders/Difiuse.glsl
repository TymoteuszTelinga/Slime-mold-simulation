#version 460 core
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D orginal;
layout(rgba32f, binding = 1) uniform image2D resoult;
layout(rgba32f, binding = 2) uniform image2D Environment;

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

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    float IsWall = imageLoad(Environment, id).r; // 1-wall/ 0-empty
    if (IsWall > 0)
    {
        return;
    }

    vec4 sum = vec4(0.0);
    float total = 0.f;
    for(int xoff = -KernelSize; xoff <= KernelSize; xoff++)
    {
        for(int yoff = -KernelSize; yoff <= KernelSize; yoff++)
        {
            ivec2 offset = ivec2(xoff,yoff);
            IsWall = imageLoad(Environment, id + offset).r;
            sum += imageLoad(orginal, id + offset) * (1.f - IsWall);
            total += (1.f - IsWall);
        }
    }
    //float total = (KernelSize * 2.f + 1.f) * (KernelSize * 2.f + 1.f);

    vec4 blureResult = sum / total;
    vec4 normal = imageLoad(orginal, id);

    vec4 pheromons = mix(normal, blureResult, DifiusonFactor * TimeStep);

    float food = imageLoad(Environment, id).g;
    pheromons += vec4(10.f) * food;//* ActiveSpeciesMask

    imageStore(resoult, id, pheromons);
}