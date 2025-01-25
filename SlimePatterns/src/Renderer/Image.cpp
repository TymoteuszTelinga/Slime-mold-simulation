#include "Image.h"
#include <GL/glew.h>

#include <fstream>

static float clamp(float val)
{
	return std::min(std::max(0.f, val), 1.f);
}

Image::Image(uint32_t Width, uint32_t Height, uint32_t Binding, float* data)
	:m_Width(Width), m_Height(Height), m_RendererID(0), m_Binding(Binding)
{
	Invalidate();

	if (data)
	{
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_FLOAT, data);
	}

}

Image::~Image()
{
	glDeleteTextures(1, &m_RendererID);
}

void Image::Bind(uint32_t Slot) const
{
	glActiveTexture(GL_TEXTURE0 + Slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Image::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::Resize(uint32_t Width, uint32_t Height, const float* data)
{
	if (Width == 0 || Height == 0)
	{
		return;
	}

	m_Width = Width;
	m_Height = Height;

	Invalidate();

	if (data)
	{
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_FLOAT, data);
	}
}

void Image::SwapBinding(Image& I1, Image& I2)
{
	glBindImageTexture(I2.m_Binding, I1.m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(I1.m_Binding, I2.m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	//std::swap(I1.m_Binding, I2.m_Binding);
	std::swap(I1.m_RendererID, I2.m_RendererID);
}

bool Image::SaveImage(const Image& image, const std::string& savePath)
{
	float* Pixels = new float[image.m_Width * image.m_Height * 4];

	glBindTexture(GL_TEXTURE_2D, image.m_RendererID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, Pixels);

	//glGetTextureImage(image.m_RendererID, 0, GL_RGBA, GL_FLOAT, image.m_Width * image.m_Height * 5, Pixels);

	const uint8_t PaddingSize = (4 - (3 * image.m_Width) % 4) % 4;
	const uint32_t PixelArraySize = image.m_Width * image.m_Height * 3 + PaddingSize * image.m_Height;

	char Padding[4] = { 0,0,0,0 };
	char BMPHeader[54] = { 0 };
	*(uint8_t*)(BMPHeader + 0x0) = 'B'; //ID
	*(uint8_t*)(BMPHeader + 0x1) = 'M'; //ID
	*(uint32_t*)(BMPHeader + 0x2) = PixelArraySize + 54; // FileSize
	*(uint32_t*)(BMPHeader + 0xA) = 54; // PixelArray offset
	//strat of DIB header
	*(uint32_t*)(BMPHeader + 0xE) = 40; // DIB Header size
	*(uint32_t*)(BMPHeader + 0x12) = image.m_Width; // width of image
	*(uint32_t*)(BMPHeader + 0x16) = image.m_Height;// height of image
	*(uint16_t*)(BMPHeader + 0x1A) = 1;  // Number of color planes being used
	*(uint16_t*)(BMPHeader + 0x1C) = 24; // Bit per pixel
	*(uint32_t*)(BMPHeader + 0x22) = PixelArraySize; // totoal size of pixel array
	*(uint32_t*)(BMPHeader + 0x26) = 2835; // Print resolution of the image horizontal
	*(uint32_t*)(BMPHeader + 0x2A) = 2835; // Print resolution of the image vertical

	std::ofstream file(savePath, std::ios::binary);
	if (!file.is_open())
	{
		delete[] Pixels;
		return false;
	}

	file.write(BMPHeader, 54);
	float* pixel = Pixels;
	for (uint32_t y = 0; y < image.m_Height; y++)
	{
		for (uint32_t x = 0; x < image.m_Width; x++)
		{
			uint8_t r = clamp(*(pixel)) * 255;
			uint8_t g = clamp(*(pixel + 1)) * 255;
			uint8_t b = clamp(*(pixel + 2)) * 255;
			pixel += 4;

			uint8_t pixel[] = { b,g,r };
			file.write(reinterpret_cast<char*>(pixel), 3);
		}
		file.write(Padding, PaddingSize);
	}

	file.close();

	delete[] Pixels;
	return true;
}

void Image::Invalidate()
{
	if (m_RendererID)
	{
		glDeleteTextures(1, &m_RendererID);
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureStorage2D(m_RendererID, 1, GL_RGBA32F, m_Width, m_Height);
	glBindImageTexture(m_Binding, m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glClearTexImage(m_RendererID, 0, GL_RGBA, GL_FLOAT, nullptr);
}
