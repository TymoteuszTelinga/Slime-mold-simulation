#include "VertexArray.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const Ref<VertexBuffer>& vb)
{
    Bind();
    vb->Bind();
    VertexBufferLayout layout = vb->GetLayout();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        glEnableVertexAttribArray(m_VertexAttribiuts + i);
        switch (element.type)
        {
        case GL_FLOAT:
        {
            glVertexAttribPointer(m_VertexAttribiuts + i, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<const void*>(offset));
            break;
        }
        case GL_INT:
        case GL_UNSIGNED_BYTE:
        {
            glVertexAttribIPointer(m_VertexAttribiuts + i, element.count, element.type, layout.GetStride(), reinterpret_cast<const void*>(offset));
            break;
        }
        }

        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);

        if (layout.bInstanceBuffer)
        {
            glVertexAttribDivisor(m_VertexAttribiuts + i, 1);
        }
    }

    m_VertexAttribiuts += elements.size();

}

void VertexArray::RemoveBuffer(const VertexBuffer& vb)
{
    Bind();
    VertexBufferLayout layout = vb.GetLayout();
    for (size_t i = m_VertexAttribiuts-1; i >= m_VertexAttribiuts-layout.GetElements().size(); i--)
    {
        glDisableVertexAttribArray(i);
    }
    m_VertexAttribiuts -= layout.GetElements().size();
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}
