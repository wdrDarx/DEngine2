#pragma once
#include "Shader.h"
#include "Core/Core.h"
#include "RenderAPI.h"
#include <glm/gtc/type_ptr.hpp>



Shader::Shader(const std::string& filepath) : m_Filepath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexShader, source.FragmentShader, source.GeometryShader);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform3fv(const std::string& name, int count, vec3d* array)
{
	glUniform3fv(GetUniformLocation(name), count, (float*)array);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform1i(const std::string& name, int i)
{
	glUniform1i(GetUniformLocation(name), i);
}

void Shader::SetUniform1f(const std::string& name, float i)
{
	glUniform1f(GetUniformLocation(name), i);
}

void Shader::SetUniform1fv(const std::string& name, int count, float* i)
{
	glUniform1fv(GetUniformLocation(name), count, i);
}

void Shader::SetUniform1iv(const std::string& name, const int& count, int* data)
{
	glUniform1iv(GetUniformLocation(name), count, data);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(GetUniformLocation(name),1,GL_FALSE, &mat[0][0]);
}


//give this a glm::value_ptr
void Shader::SetUniformMat4fv(const std::string& name, int count, const float* mat)
{
	glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, mat);
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
	};


	std::string line;
	std::stringstream ss[3];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else
				if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
				else
					if (line.find("geometry") != std::string::npos)
						type = ShaderType::GEOMETRY;
		}
		else
			ss[(int)type] << line << "\n";
	}
	return { ss[0].str(),ss[1].str(), ss[2].str() };
}


uint Shader::CompileShader(uint type, const std::string& source)
{
	uint id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		std::cout << "Failed to compile shader! ";
		std::cout << message;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

uint Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
{
	uint program = glCreateProgram();
	uint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	uint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	uint gs = 0;

	if(!geometryShader.empty())
	{ 
		gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
	}

	glAttachShader(program, vs);

	if (!geometryShader.empty())
	{ 
		glAttachShader(program, gs);
	}

 	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	if (!geometryShader.empty())
	{ 
		glDeleteShader(gs);
	}

	return program;
};

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	m_UniformLocationCache[name] = location;

	return location;
}
