#pragma once
#include "VertexBufferLayout.h"

class VertexBuffer
{
public:
    VertexBuffer(const void* data, unsigned int size, VertexBufferLayout layout);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    void SetData(const void* data, unsigned int size);
    VertexBufferLayout GetLayout() const { return m_BufferLayout; };

private:
    unsigned int m_RendererID;
    VertexBufferLayout m_BufferLayout;
};
