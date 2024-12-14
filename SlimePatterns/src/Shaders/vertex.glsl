#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 UV;

uniform vec2 Scale;

out vec2 TexUV;

void main()
{
	gl_Position = vec4(Scale * position, 0.0, 1.0);
	TexUV = UV;
}