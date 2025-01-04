#version 460 core

layout(location = 0) out vec4 color;

in vec2 TexUV;

layout(binding = 0) uniform sampler2D uTexture;

uniform vec3 Mask;
uniform bool GrayScale;

vec3 spectral_bruton(float w)
{
	vec3 c;

	if (w >= 380. && w < 440.)
		c = vec3
		(
			-(w - 440.) / (440. - 380.),
			0.0,
			1.0
		);
	else if (w >= 440. && w < 490.)
		c = vec3
		(
			0.0,
			(w - 440.) / (490. - 440.),
			1.0
		);
	else if (w >= 490. && w < 510.)
		c = vec3
		(0.0,
			1.0,
			-(w - 510.) / (510. - 490.)
		);
	else if (w >= 510. && w < 580.)
		c = vec3
		(
			(w - 510.) / (580. - 510.),
			1.0,
			0.0
		);
	else if (w >= 580. && w < 645.)
		c = vec3
		(
			1.0,
			-(w - 645.) / (645. - 580.),
			0.0
		);
	else if (w >= 645. && w <= 780.)
		c = vec3
		(1.0,
			0.0,
			0.0
		);
	else
		c = vec3
		(0.0,
			0.0,
			0.0
		);

	return clamp(c,0.f,1.f);
}

vec3 gradient(float value)
{
    vec3 color1 = vec3(0.098, 0.094, 0.090);
    vec3 color2 = vec3(0.470, 0.352, 0.274);
    vec3 color3 = vec3(0.509, 0.094, 0.090);
    vec3 color4 = vec3(0.980, 0.701, 0.392);
    vec3 color5 = vec3(0.168, 0.254, 0.384);
    vec3 color6 = vec3(0.043, 0.431, 0.309);
    vec3 color7 = vec3(0.588, 0.431, 0.309);
    vec3 color8 = vec3(1, 1, 1);

    vec3 col;
    if (value < 0.03) {
        float range = 0.03;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else if (value < 0.05) {
        float range = 0.02;
        float pos = value - 0.03;
        float value = pos / range;
        col = mix(color2, color3, value);
    }
    else if (value < 0.25) {
        float range = 0.20;
        float pos = value - 0.05;
        float value = pos / range;
        col = mix(color3, color4, value);
    }
    else if (value < 0.50) {
        float range = 0.25;
        float pos = value - 0.25;
        float value = pos / range;
        col = mix(color4, color5, value);
    }
    else if (value < 0.85) {
        float range = 0.35;
        float pos = value - 0.50;
        float value = pos / range;
        col = mix(color5, color6, value);
    }
    else if (value < 0.95) {
        float range = 0.10;
        float pos = value - 0.85;
        float value = pos / range;
        col = mix(color6, color7, value);
    }
    else {
        float range = 0.05;
        float pos = value - 0.95;
        float value = pos / range;
        col = mix(color7, color8, value);
    }

    return col;
}

void main()
{
    vec3 texColor = texture(uTexture, TexUV).xyz;
    vec3 total = vec3(0.f);
    if (GrayScale)
    {
		float w = dot(texColor, vec3(1.f)) / 3.f;
		//w = w * 300.f + 400.f;
		total = w * vec3(0.1, 0.6, 0.3);//spectral_bruton(w);
        total = gradient(w);

    }
    else
    {
        total = texColor * Mask;
    }
    color = vec4(total, 1.f);
}