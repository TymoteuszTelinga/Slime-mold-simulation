#version 460 core
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D Texture;


//Mainfunction
void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    vec4 orginal = imageLoad(Texture, id);
    orginal = max(vec4(0), orginal - 0.01);

    imageStore(Texture, id, orginal);
}