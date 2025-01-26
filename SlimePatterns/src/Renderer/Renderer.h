#pragma once
#include "Image.h"
#include "Shader.h"
#include "Core/Base.h"

class Renderer
{
public:
	static void Init();

	static void Resize(int width, int height);

	static void Clear();

	static void DisplayImage(const Ref<Image> image, const vec3& mask, bool useGrayScale, int32_t activeSpices);

	static void DisplayImage(const Ref<Image>& image, const Ref<Shader>& shader);
};
