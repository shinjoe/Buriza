#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GLM/gtc/type_ptr.hpp>

Shader::Shader(const GLchar* path)
    : Shader((std::string(path) + ".vs").c_str(), (std::string(path) + ".fs").c_str())
{ }

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    using std::string;
    using std::ifstream;

    string vertexCode{};
    string fragmentCode{};
    ifstream vShaderFile{};
    ifstream fShaderFile{};
    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (ifstream::failure&)
    {
        std::cerr << "Cannot read shader files!" << std::endl;
    }

    const GLchar* vertexShaderCode = vertexCode.c_str();
    const GLchar* fragmentShaderCode = fragmentCode.c_str();

    CompileShader(vertexShaderCode, fragmentShaderCode);
}


void Shader::Use() const
{
    glUseProgram(m_program);
}

GLuint Shader::getProgram() const
{
    return m_program;
}

void Shader::SetVec3(const char* target, GLfloat x, GLfloat y, GLfloat z) const
{
    glUniform3f(glGetUniformLocation(m_program, target), x, y, z);
}

void Shader::SetVec3(const char* target, const glm::vec3& v) const
{
    glUniform3fv(glGetUniformLocation(m_program, target), 1, glm::value_ptr(v));
}

void Shader::SetMat4(const char* target, const glm::mat4& matrix) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, target), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetFloat(const char* target, GLfloat val) const
{
    glUniform1f(glGetUniformLocation(m_program, target), val);
}

void Shader::SetInt(const char* target, GLint val) const
{
    glUniform1i(glGetUniformLocation(m_program, target), val);
}

void Shader::CompileShader(const GLchar* vertexShaderCode, const GLchar* fragmentShaderCode)
{
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader compilation error" << std::endl << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader compilation error" << std::endl << infoLog << std::endl;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_program, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed" << std::endl << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
