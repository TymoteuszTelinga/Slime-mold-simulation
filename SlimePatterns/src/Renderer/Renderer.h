#pragma once
#include "Image.h"
#include "Core/Base.h"

class Renderer
{
public:
	static void Init();

	static void Resize(int width, int height);

	static void Clear();

	static void DisplayImage(const Ref<Image> image);
};
