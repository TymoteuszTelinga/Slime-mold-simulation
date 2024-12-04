#pragma once
#include <GL/glew.h>
#include <vector>

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT: return 4;
        case GL_INT: return 4;
        case GL_UNSIGNED_BYTE: return 1;
        }
        return 0;
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() : m_Stride(0) {};
    ~VertexBufferLayout() {};

    void Pushf(unsigned int count)
    {
        m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
    }

    void Pushi(unsigned int count)
    {
        m_Elements.push_back({ GL_INT, count, GL_FALSE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_INT) * count;
    }

    void Pushc(unsigned int count)
    {
        m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
    }

    inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; };
    inline unsigned int GetStride() const { return m_Stride; };

    bool bInstanceBuffer = false;
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
};
