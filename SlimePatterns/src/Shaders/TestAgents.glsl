#version 460 core
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D Texture;
layout(rgba32f, binding = 2) uniform image2D Boundry;

struct Agent
{
    vec2 pos;
    float angle;
    int SpeciesIndex;
    ivec4 mask;
};

uniform uint Agents;
layout(std430, binding = 0) buffer bAgent
{
    Agent agents[];
};

//General Parameters
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

struct SpeciesSettings
{
    vec4 SpeciesMask;
    float SensorAngle;
    float SensorOffset;
    int SensorSize;
    float AgentSpeed;
    float AgentRotation;
    float Deposit;
    vec2 _pading;
};

layout(std430, binding = 2) buffer bSpecies
{
    SpeciesSettings Species[3];
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

//Helper functions
float sense(in Agent agent, in SpeciesSettings settings, float sensorAngleOffset)
{
    float sensorAngle = agent.angle + sensorAngleOffset;
    vec2 sensorDir = vec2(cos(sensorAngle), sin(sensorAngle));

    ivec2 sensorPos = ivec2(agent.pos + sensorDir * settings.SensorOffset);

    float sum = 0;
    vec4 senseWeight = settings.SpeciesMask; /*vec4(agent.mask * 2.f - 1.f);*/

    for (int xoff = -settings.SensorSize; xoff <= settings.SensorSize; xoff++)
    {
        for (int yoff = -settings.SensorSize; yoff <= settings.SensorSize; yoff++)
        {
            ivec2 offset = ivec2(xoff, yoff);
            sum += dot(senseWeight, imageLoad(Texture, sensorPos + offset) );
        }
    }

    return sum;
}

//Mainfunction
void main()
{
    uint id = gl_GlobalInvocationID.x;
    rngSeed = id + RandSeed;
    if (id >= Agents)
    {
        return;
    }

    // collect sensor data
    Agent agent = agents[id];
    SpeciesSettings settings = Species[agent.SpeciesIndex];
    float weightForward = sense(agent, settings, 0);
    float weightLeft = sense(agent, settings, settings.SensorAngle);
    float weightRight = sense(agent, settings, -settings.SensorAngle);


    // Continue in same direction
    if (weightForward > weightLeft && weightForward > weightRight)
    {
        agent.angle += 0;
    }
    else if (weightForward < weightLeft && weightForward < weightRight) 
    {
        agent.angle += (rand() - 0.5) * 2 * settings.AgentRotation * TimeStep;
    }
    // Turn right
    else if (weightRight > weightLeft) 
    {
        agent.angle -= settings.AgentRotation * TimeStep;
    }
    // Turn left
    else if (weightLeft > weightRight) 
    {
        agent.angle += settings.AgentRotation * TimeStep;
    }


    //Move In Direction;
    vec2 dir = vec2(cos(agent.angle), sin(agent.angle));
    vec2 agentPosition = agent.pos + (dir * settings.AgentSpeed * TimeStep);

    


    //left side collision
    if (agentPosition.x <= 0)
    {
        agent.angle = rand() * 3.14f - (3.14f/2.f);
    }
    //right side collision
    if (agentPosition.x >= Width)
    {
        agent.angle = rand() * 3.14f + (3.14f / 2.f);
    }
    //bottom side collision
    if (agentPosition.y <= 0)
    {
        agent.angle = rand() * 3.14f;
    }
    //top side collision
    if (agentPosition.y >= Height)
    {
        agent.angle = rand() * 3.14f + 3.14f;
    }

    //set agent position inside image box
    agentPosition.x = min(Width - 0.01, max(0.01, agentPosition.x));
    agentPosition.y = min(Height - 0.01, max(0.01, agentPosition.y));

    float IsWall = imageLoad(Boundry, ivec2(agentPosition)).r;
    if (IsWall > 0)
    {
        agentPosition = agent.pos;//UnDo move;
        agent.angle = rand() * 3.14f * 2f; //New random rotation;
    }

    //update agent position
    agent.pos = agentPosition;
    agents[id] = agent;

    vec4 old = imageLoad(Texture, ivec2(agents[id].pos));
    //vec4 deposit = min(vec4(1.f), old + vec4(agent.mask) * settings.Deposit * TimeStep);
    vec4 deposit = max(old, min(vec4(1.f), old + vec4(agent.mask) * settings.Deposit * TimeStep ));
    imageStore(Texture, ivec2(agents[id].pos), deposit);
}