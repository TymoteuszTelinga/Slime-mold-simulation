#version 460 core
layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D Texture;


//Move to uniform buffer
const int sensorSize = 1;//how big area of sensor is (haf size)

layout(std140, binding = 1) buffer Parameters
{
    float TimeStep;
    //Pheromons params
    int KernelSize;
    float EvaporationFactor;
    //Agent params
    float SensorAngle;
    float SensorOffset;
    int SensorSize;
    float AgentSpeed;
    float AgentRotation;
};

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

//Helper functions
float sense(Agent agent, float sensorAngleOffset) {
    float sensorAngle = agent.angle + sensorAngleOffset;
    vec2 sensorDir = vec2(cos(sensorAngle), sin(sensorAngle));

    vec2 sensorPos = agent.pos + sensorDir * SensorOffset;

    float sum = 0;

    ivec4 senseWeight = ivec4(1) * 2 - 1;

    for (int xoff = -sensorSize; xoff <= sensorSize; xoff++) {
        for (int yoff = -sensorSize; yoff <= sensorSize; yoff++) {
            ivec2 offset = ivec2(xoff, yoff);
            sum += dot(senseWeight, imageLoad(Texture, ivec2(sensorPos) + offset));
        }
    }

    return sum;
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

    // collect sensor data
    float weightForward = sense(agents[id], 0);
    float weightLeft = sense(agents[id], SensorAngle);
    float weightRight = sense(agents[id], -SensorAngle);

    float randomSteerStrength = rand();

    // Continue in same direction
    if (weightForward > weightLeft && weightForward > weightRight) {
        agents[id].angle += 0;
    }
    else if (weightForward < weightLeft && weightForward < weightRight) {
        agents[id].angle += (randomSteerStrength - 0.5) * 2 * AgentRotation * TimeStep;
    }
    // Turn right
    else if (weightRight > weightLeft) {
        agents[id].angle -= randomSteerStrength * AgentRotation * TimeStep;
    }
    // Turn left
    else if (weightLeft > weightRight) {
        agents[id].angle += randomSteerStrength * AgentRotation * TimeStep;
    }


    //Move In Direction;
    vec2 dir = vec2(cos(agents[id].angle), sin(agents[id].angle));
    vec2 agentPosition = agents[id].pos + (dir * AgentSpeed * TimeStep);

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

    //set agent position inside image box
    agentPosition.x = min(width - 0.01, max(0, agentPosition.x));
    agentPosition.y = min(height - 0.01, max(0, agentPosition.y));

    //update agent position
    agents[id].pos = agentPosition;

    vec4 color = vec4(1.0);
    imageStore(Texture, ivec2(agents[id].pos), color);
}