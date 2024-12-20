#include "shader.h"
#include <glad/glad.h>
#include <gtc/type_ptr.hpp>
#include <iostream>

int Shader::getLocation(const std::string& uniform)
{
    auto it = m_map.find(uniform);
 
    // Return existing shader locatoin
    if (it != m_map.end())
    {
        return it->second.loc;
    }

    // Query and insert the uniform location
    m_map[uniform] = { glGetUniformLocation(m_programId, uniform.c_str()) };
    return m_map[uniform].loc;
}

Shader::Shader(const std::string& vert, const std::string& frag)
{
    load(vert, frag);
}

void Shader::setInt(const std::string& uniform, int value)
{
    glUniform1i(getLocation(uniform), value);
}

void Shader::setFloat(const std::string& uniform, float value)
{
    glUniform1f(getLocation(uniform), value);
}

void Shader::setMat4(const std::string& uniform, bool transpose, const glm::mat4& mat)
{
    glUniformMatrix4fv(getLocation(uniform), 1, transpose, glm::value_ptr(mat));
}

void Shader::setVec2(const std::string& uniform, float a, float b)
{
    glUniform2f(getLocation(uniform), a, b);
}

void Shader::bind() const
{
    glUseProgram(m_programId);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

void checkCompileErrors(GLuint shader, const std::string& type)
{
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "[ERROR] Shader compilation error: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void Shader::load(const std::string& vert, const std::string& frag)
{
    const char* vcstr = vert.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vcstr, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    const char* fcstr = frag.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fcstr, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertexShader);
    glAttachShader(m_programId, fragmentShader);
    glLinkProgram(m_programId);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader Shader::loadDefault()
{
    return Shader();
}

Shader::~Shader()
{
    glDeleteProgram(m_programId);
}
