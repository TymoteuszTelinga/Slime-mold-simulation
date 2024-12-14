#pragma once

#include <unordered_map>
#include <string>

class Shader
{
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void Bind() const;
    void Unbind() const;
    uint32_t GetRendererID() const { return m_RendererID; };

    //Set uniforms
    void SetUniform1i(const std::string& name, int v0);
    void SetUniform1f(const std::string& name, float v0);
    void SetUniform2f(const std::string& name, float v0, float v1);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
    int GetUniformLocation(const std::string& name);
    std::string loadShaderFromFile(const std::string& filePath) const;
    unsigned int compileShader(unsigned int type, const std::string& source) const;
    unsigned int createShader(const std::string& VertexShader, const std::string& FragmentShader) const;
private:
    uint32_t m_RendererID;
    //cashin for uniforms
    std::unordered_map<std::string, int> m_UniformLocationCache;
};