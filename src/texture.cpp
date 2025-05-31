#include "texture.h"


#include <iostream>
#include <cmath>
#include <algorithm>

#include <glad/glad.h>
#include <stb_image.h>



namespace mk
{


	Texture::~Texture()
	{

	}

	bool Texture::Create(const mk::TextureCreateInfo& textureInfo)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);

		int desiredChannels = textureInfo.desiredChannels == MK_TEXTURE_DEFAULT_CHANNELS ? 0 : textureInfo.desiredChannels;

		uint8_t* pixels = stbi_load(textureInfo.texturePath.c_str(), &width, &height, &channels, desiredChannels);
		if (!pixels)
		{
			std::cerr << "Failed to load [" << textureInfo.texturePath << "] : " << stbi_failure_reason() << "\n";
			return false;
		}

		int internalFormat = GL_RGBA8;
		int dataFormat = GL_RGBA;

		switch (channels)
		{
		case 1:
			internalFormat = GL_R8;
			dataFormat = GL_RED; 
			break;

		case 2:
			internalFormat = GL_RG8;
			dataFormat = GL_RG;
			break;

		case 3:
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			break;

		case 4:
		default:
			break;
		}

		//int mipLevels = (int)std::floor(std::log2(std::max(width, height))) + 1;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
		glTextureStorage2D(m_id, 1, internalFormat, width, height);
		//glTextureStorage2D(m_id, mipLevels, internalFormat, width, height);

		glTextureSubImage2D(m_id, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, pixels);

		for (uint32_t i = 0; i < textureInfo.paramCount; i++)
		{
			const auto& param = textureInfo.pParams[i];
			glTextureParameteri(m_id, param.name, param.value);
		}

		if (channels == 1)
		{
			int swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
			glTextureParameteriv(m_id, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		}

		float maxAniso;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
		glTextureParameterf(m_id, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

		//glGenerateTextureMipmap(m_id);

		stbi_image_free(pixels);

		return true;
	}

	void Texture::Bind() const
	{
		glBindTextureUnit(m_unit, m_id);
	}

}
