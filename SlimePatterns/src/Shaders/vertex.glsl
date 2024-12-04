#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 UV;

out vec2 TexUV;

void main()
{
	gl_Position = position;
	TexUV = UV;
}