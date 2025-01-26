#version 460 core

layout(location = 0) out vec4 color;

in vec2 TexUV;

layout(binding = 0) uniform sampler2D uTexture;
layout(binding = 1) uniform sampler2D uBoundry;

uniform vec3 Mask;
uniform bool GrayScale;
uniform float ActiveSpices;

uniform vec3 ColorLow;
uniform vec3 ColorHigh;

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
    //vec3 color1 = vec3(0.098, 0.094, 0.090);
    vec3 color1 = vec3(0.06, 0.06, 0.06);
    vec3 color2 = vec3(0.470, 0.352, 0.274);
    vec3 color3 = vec3(0.509, 0.094, 0.090);
    vec3 color4 = vec3(0.980, 0.701, 0.392);
    vec3 color5 = vec3(0.168, 0.254, 0.384);
    vec3 color6 = vec3(0.043, 0.431, 0.309);
    vec3 color7 = vec3(0.588, 0.431, 0.309);
    vec3 color8 = vec3(0.588, 0.431, 0.309);
    /*vec3 color8 = vec3(1, 1, 1);*/

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
//M
vec3 gradient1(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.006, 0.006, 0.006);
    vec3 color2 = vec3(0.972, 0.607, 0.160);
    vec3 color3 = vec3(1.f, 0.058, 0.482);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//O
vec3 gradient2(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.06, 0.06, 0.06);
    vec3 color3 = vec3(0.894, 0.654, 0.772);
    vec3 color2 = vec3(0.427, 0.317, 0.647);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//O
vec3 gradient3(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.006, 0.006, 0.006);
    vec3 color2 = vec3(0.909, 0.109, 1.000);
    vec3 color3 = vec3(0.250, 0.788, 1.000);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//M
vec3 gradient4(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.06, 0.06, 0.06);
    vec3 color2 = vec3(0.0, 1.0, 0.529);
    vec3 color3 = vec3(0.376, 0.937, 1.0);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//M
vec3 gradient5(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.006, 0.006, 0.006);
    vec3 color2 = vec3(1.0, 0.105, 0.419);
    vec3 color3 = vec3(0.270, 0.7928, 1.0);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//G light blue
vec3 gradient6(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.03, 0.03, 0.03);
    vec3 color2 = vec3(0.078, 0.321, 0.466);
    vec3 color3 = vec3(0.513, 0.815, 0.796);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//M
vec3 gradient7(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.06, 0.06, 0.06);
    vec3 color2 = vec3(0.709, 0.776, 0.878);
    vec3 color3 = vec3(0.921, 0.956, 0.960);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//G red
vec3 gradient8(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.06, 0.06, 0.06);
    vec3 color2 = vec3(0.901, 0.137, 0.0784);
    vec3 color3 = vec3(0.945, 0.619, 0.0941);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//O
vec3 gradient9(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.06, 0.06, 0.06);
    vec3 color2 = vec3(0.741, 0.654, 0.203);
    vec3 color3 = vec3(0.968, 0.949, 0.670);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//G dark blue
vec3 gradient10(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.06, 0.06, 0.06);
    vec3 color2 = vec3(0.00784313725490196, 0.011764705882352941, 0.26666666666666666);
    vec3 color3 = vec3(0.1568627450980392, 0.7215686274509804, 0.8352941176470589);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}
//G green
vec3 gradient11(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 color1 = vec3(0.06, 0.06, 0.06);
    vec3 color2 = vec3(0.23137254901960785, 0.3411764705882353, 0.0);
    //vec3 color3 = vec3(0.40784313725490196, 0.5529411764705883, 0.08235294117647059);
    //vec3 color3 = vec3(0.596078431372549, 0.8196078431372549, 0.12156862745098039);
    vec3 color3 = vec3(0.7843137254901961, 1.0, 0.3215686274509804);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(color1, color2, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(color2, color3, value);
    }

    return col;
}

vec3 gradientCustom(float value)
{
    value = clamp(value, 0.f, 1.f);
    vec3 colorBG = vec3(0.06, 0.06, 0.06);

    vec3 col;
    if (value < 0.20) {
        float range = 0.20;
        float pos = value - 0.0;
        float value = pos / range;
        col = mix(colorBG, ColorLow, value);
    }
    else {
        float range = 0.80;
        float pos = value - 0.20;
        float value = pos / range;
        col = mix(ColorLow, ColorHigh, value);
    }

    return col;
}


void main()
{
    vec3 texColor = texture(uTexture, TexUV).xyz;
    float boundry = texture(uBoundry, TexUV).r;
    float food = texture(uBoundry, TexUV).g;

    vec3 total = vec3(0.f);
    if (GrayScale)
    {
		float w = dot(texColor, vec3(1.f)) / ActiveSpices;
		//w = w * 300.f + 400.f;
		//total = w * vec3(0.1, 0.6, 0.3);//spectral_bruton(w);
        //w = clamp(w, 0.f, 1.f);
        //total = gradient(w);
        total = gradientCustom(w);
        //total = vec3(w);

    }
    else
    {
        total = texColor * Mask;
    }

    //feromones
    //if (dot(texColor, vec3(0.f,0.f,1.f)) > 0)
    //{
    //    total.b = 1.f;
    //    //total = vec3(Mask);
    //}

    total = mix(total, vec3(0.f), boundry);
    total = mix(total, vec3(1, 1, 1), food);
    color = vec4(total, 1.f);
}