#pragma once

#include <cstdint>

class Buffer
{
public:
	Buffer(uint64_t size, uint32_t Binding);
	~Buffer();

	void SetData(const void* data, uint64_t size, uint32_t offset = 0);

private:
	uint32_t m_RendererID;
};
