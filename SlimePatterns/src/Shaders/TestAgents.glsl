#version 460 core
layout(local_size_x = 10, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D Texture;

struct Agent
{
    vec2 pos;
    float angle;
    float w;
};

uniform uint Agents;
layout(std430, binding = 0) buffer bAgent
{
    Agent agents[];
};

//Random
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

//Mainfunction
void main()
{
    uint id = gl_GlobalInvocationID.x;
    rngSeed = id;
    if (id >= Agents)
    {
        return;
    }

    float width = imageSize(Texture).x;
    float height = imageSize(Texture).y;

    //Move In Direction;
    vec2 dir = vec2(cos(agents[id].angle), sin(agents[id].angle));
    vec2 agentPosition = agents[id].pos + dir;

    //left side collision
    if (agentPosition.x <= 0)
    {
        agents[id].angle = rand() * 3.14f - (3.14f/2.f);
    }
    //right side collision
    if (agentPosition.x >= width)
    {
        agents[id].angle = rand() * 3.14f + (3.14f / 2.f);
    }
    //bottom side collision
    if (agentPosition.y <= 0)
    {
        agents[id].angle = rand() * 3.14f;
    }
    //top side collision
    if (agentPosition.y >= height)
    {
        agents[id].angle = rand() * 3.14f + 3.14f;
    }

    //set agent posirtion inside image box
    agentPosition.x = min(width - 0.01, max(0, agentPosition.x));
    agentPosition.y = min(height - 0.01, max(0, agentPosition.y));

    //update agent position
    agents[id].pos = agentPosition;

    vec4 color = vec4(1.0);
    imageStore(Texture, ivec2(agents[id].pos), color);
}