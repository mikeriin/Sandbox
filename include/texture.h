#ifndef MK_TEXTURE_H
#define MK_TEXTURE_H


#include <cstdint>
#include <string>

#include "globals.h"


namespace mk
{

	struct TextureParameters
	{
		uint32_t name;
		int value;
	};


	struct TextureCreateInfo
	{
		std::string texturePath;
		int desiredChannels = MK_TEXTURE_DEFAULT_CHANNELS;
		uint32_t unit;
		uint32_t paramCount;
		mk::TextureParameters* pParams;
	};



	class Texture
	{
	private:
		uint32_t m_id = 0;
		uint32_t m_unit = 0;


	public:
		Texture() = default;
		~Texture();


		bool Create(const mk::TextureCreateInfo& textureInfo);


		void Bind() const;


		inline uint32_t GetID() const { return m_id; }
		inline uint32_t GetUnit() const { return m_unit; }
	};


}



#endif // !MK_TEXTURE_H
