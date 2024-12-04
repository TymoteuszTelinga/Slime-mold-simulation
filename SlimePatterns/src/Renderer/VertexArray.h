#pragma once

#include "VertexBuffer.h"
#include "Core/Base.h"

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const Ref<VertexBuffer>& vb);
    void RemoveBuffer(const VertexBuffer& vb);
    unsigned int GetAttribCount() const { return m_VertexAttribiuts; };

    void Bind() const;
    void Unbind() const;
private:
    unsigned int m_RendererID;
    unsigned int m_VertexAttribiuts = 0;
};