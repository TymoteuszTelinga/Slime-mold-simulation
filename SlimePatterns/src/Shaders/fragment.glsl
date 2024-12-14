#version 460 core

layout(location = 0) out vec4 color;

in vec2 TexUV;

layout(binding = 0) uniform sampler2D uTexture;

void main()
{
    vec3 texColor = texture(uTexture, TexUV).xyz;
    vec3 total = vec3(dot(texColor, vec3(1.f))/3.f);
    color = vec4(total, 1.f);
}