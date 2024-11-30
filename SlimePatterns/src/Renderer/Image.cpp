#include "Image.h"
#include <GL/glew.h>

Image::Image(uint32_t Width, uint32_t Height)
	:m_Width(Width), m_Height(Height), m_RendererID(0)
{
	Invalidate();
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

void Image::Invalidate()
{
	if (m_RendererID)
	{
		glDeleteTextures(1, &m_RendererID);
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureStorage2D(m_RendererID, 1, GL_RGBA32F, m_Width, m_Height);
	glBindImageTexture(0, m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
