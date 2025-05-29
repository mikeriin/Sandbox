#ifndef MK_SHADER_H
#define MK_SHADER_H


#include <cstdint>
#include <string>
#include <vector>

#include <glm.hpp>



namespace mk
{


	struct ShaderInfo
	{
		std::string path;
		uint32_t type;
	};


	struct ShaderProgramCreateInfo
	{
		uint32_t shaderCount;
		mk::ShaderInfo* pShaders;
	};



	class ShaderProgram
	{
	private:
		uint32_t m_id = 0;
		std::vector<uint32_t> m_shaders = {};


	public:
		ShaderProgram() = default;
		~ShaderProgram();


		bool Create(const mk::ShaderProgramCreateInfo& programInfo);

		void Use() const;


		void SetVec3(const char* location, const glm::vec3& v) const;
		void SetTexture(const char* location, int unit);


	private:
		std::string readFile(const std::string& filepath) const;
		uint32_t createShader(const mk::ShaderInfo& shader);


	};


}




#endif // !MK_SHADER_H
