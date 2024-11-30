#version 460 core
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D Texture;

uint rngSeed;

uint PCGHash()
{
    rngSeed = rngSeed * 747796405u + 2891336453u;
    uint state = rngSeed;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rand() //random float from 0 to 1
{
    return PCGHash() / float(0xFFFFFFFFU);
}

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    rngSeed = uint(id.y * imageSize(Texture).x + id.x);

    vec4 color = vec4(vec3(rand()), 1.f);
    imageStore(Texture, id, color);
}