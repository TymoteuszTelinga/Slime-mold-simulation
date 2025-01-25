#version 460 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D FeromonTrail;

layout(std140, binding = 1) buffer Parameters
{
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
    uint id = gl_GlobalInvocationID.x;

    ivec2 position = ivec2(400, 100);

    vec4 food = vec4(10);

    imageStore(FeromonTrail, position, food);
    imageStore(FeromonTrail, ivec2(700,300), vec4(100));
}