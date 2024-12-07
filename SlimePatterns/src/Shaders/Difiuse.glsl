#version 460 core
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D orginal;
layout(rgba32f, binding = 1) uniform image2D resoult;

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

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    vec4 sum = vec4(0.0);
    for(int xoff = -1; xoff <= 1; xoff++)
    {
        for(int yoff = -1; yoff <= 1; yoff++)
        {
            ivec2 offset = ivec2(xoff,yoff);
            sum += imageLoad(orginal, id + offset);
        }
    }

    vec4 blureResult = sum / 9;

    imageStore(resoult, id, blureResult);
}