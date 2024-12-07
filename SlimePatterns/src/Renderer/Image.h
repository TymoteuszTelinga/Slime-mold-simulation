#pragma once

#include <string>

class Image
{
public:
	Image(uint32_t Width, uint32_t Height, uint32_t Binding = 0);
	~Image();

	void Bind(uint32_t Slot = 0) const;
	void UnBind() const;

	void Resize(uint32_t Width, uint32_t Height);

	inline uint32_t GetRendererID() const { return m_RendererID; };
	inline uint32_t GetWidth() const { return m_Width; };
	inline uint32_t GetHeight() const { return m_Height; };

	static void SwapBinding(Image& I1, Image&I2);

private:

	void Invalidate();

private:
	uint32_t m_RendererID;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_Binding;
};
