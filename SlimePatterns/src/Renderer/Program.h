#pragma once

#include <string>
#include <unordered_map>

class Program
{
public:
    Program(const std::string& ComputePath);
    ~Program();

    void Bind() const;
    void Unbind() const;

    void Dispatch(uint32_t SizeX, uint32_t SizeY);

    //Set uniforms
    void SetUniform1i(const std::string& name, int32_t v0);

private:
    int32_t GetUniformLocation(const std::string& name);
    std::string LoadProgramFromFile(const std::string& filePath) const;
    uint32_t CompileProgram(const std::string& source) const;
    uint32_t CreateProgram(const std::string& ComputeShader) const;

private:
    uint32_t m_RendererID;
    //cashin for uniforms
    std::unordered_map<std::string, int32_t> m_UniformLocationCache;
};