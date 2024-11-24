#pragma once
#include <string>
#include <unordered_map>
#include <mat4x4.hpp>

class Shader
{
private:
    unsigned int m_programId;

    struct ShaderData
    {
        int loc;
    };

    std::unordered_map<std::string, ShaderData> m_map;

    int getLocation(const std::string& uniform);

public:
    Shader(const std::string& vert, const std::string& frag);

    Shader() = default;

    void setInt(const std::string& uniform, int value);

    void setFloat(const std::string& uniform, float value);

    void setMat4(const std::string& uniform, bool transpose, const glm::mat4& mat);

    void bind() const;

    void unbind() const;

    void load(const std::string& vert, const std::string& frag);

    static Shader loadDefault();

    ~Shader();
};