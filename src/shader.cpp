#include "shader.h"


#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>



namespace mk
{



	ShaderProgram::~ShaderProgram()
	{
		for (uint32_t shader : m_shaders)
			if (shader) glDeleteShader(shader);

		if (m_id) glDeleteProgram(m_id);
	}

	bool ShaderProgram::Create(const mk::ShaderProgramCreateInfo& programInfo)
	{
		m_id = glCreateProgram();
		m_shaders.resize(programInfo.shaderCount, 0);

		for (size_t i = 0; i < programInfo.shaderCount; i++)
		{
			const auto& shaderInfo = programInfo.pShaders[i];
			
			uint32_t shader = createShader(shaderInfo);
			if (!shader) return false;
			m_shaders[i] = shader;

			glAttachShader(m_id, shader);
		}

		glLinkProgram(m_id);

		int isLinked;
		glGetProgramiv(m_id, GL_LINK_STATUS, &isLinked);
		if (!isLinked)
		{
			int maxLength = 0;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

			char* infoLog = new char[maxLength];
			glGetProgramInfoLog(m_id, maxLength, &maxLength, infoLog);

			std::cerr << "Failed to link program: " << infoLog << "\n";

			glDeleteProgram(m_id);
			for (uint32_t shader : m_shaders) glDeleteShader(shader);

			return false;
		}

		for (uint32_t shader : m_shaders) glDetachShader(m_id, shader);

		return true;
	}

	void ShaderProgram::Use() const
	{
		glUseProgram(m_id);
	}

	void ShaderProgram::SetVec3(const char* location, const glm::vec3& v) const
	{
		glUniform3fv(glGetUniformLocation(m_id, location), 1, &v[0]);
	}

	void ShaderProgram::SetTexture(const char* location, int unit)
	{
		glUniform1i(glGetUniformLocation(m_id, location), unit);
	}

	std::string ShaderProgram::readFile(const std::string& filepath) const
	{
		std::ifstream shaderFile(filepath);

		if (!shaderFile.is_open())
		{
			std::cerr << "Failed to open [" << filepath << "]\n";
			return "";
		}

		std::ostringstream buffer;
		buffer << shaderFile.rdbuf();

		return buffer.str();
	}

	uint32_t ShaderProgram::createShader(const mk::ShaderInfo& shader)
	{
		uint32_t id = glCreateShader(shader.type);
		std::string shaderSourceStr = readFile(shader.path);
		const char* shaderSource = shaderSourceStr.c_str();
		glShaderSource(id, 1, &shaderSource, nullptr);
		glCompileShader(id);

		int isCompiled;
		glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
		if (!isCompiled)
		{
			int maxLength = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

			char* infoLog = new char[maxLength];
			glGetShaderInfoLog(id, maxLength, &maxLength, infoLog);

			std::cerr << "Failed to create shader: " << infoLog << "\n";

			glDeleteShader(id);
		}

		return id;
	}

}