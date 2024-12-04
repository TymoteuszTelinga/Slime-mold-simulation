#pragma once

class Renderer
{
public:
	static void Init();

	static void Resize(int width, int height);

	static void Clear();

	static void DisplayImage();
};
