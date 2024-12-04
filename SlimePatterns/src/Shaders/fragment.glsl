#version 460 core

layout(location = 0) out vec4 color;

in vec2 TexUV;

layout(binding = 0) uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, TexUV);
    color = vec4(texColor);
}