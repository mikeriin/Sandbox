#include "buffer.h"


#include <iostream>

#include <glad/glad.h>



namespace mk
{


	Buffer::~Buffer()
	{
		if (m_id) glDeleteBuffers(1, &m_id);
	}


	bool Buffer::Create(const mk::BufferCreateInfo& bufferInfo)
	{
		m_size = bufferInfo.size;
		m_isMutable = bufferInfo.isMutable;

		glCreateBuffers(1, &m_id);
		
		if (m_isMutable) 
			glNamedBufferData(m_id, m_size, bufferInfo.pData, bufferInfo.usage);
		else 
			glNamedBufferStorage(m_id, m_size, bufferInfo.pData, bufferInfo.flags);

		if (bufferInfo.binding != MK_NO_BUFFER_BINDING_INDEX) 
			glBindBufferBase(bufferInfo.target, bufferInfo.binding, m_id);

		return true;
	}


	void Buffer::Update(const mk::BufferUpdateInfo& updateInfo) const
	{
		if (updateInfo.offset + updateInfo.size > m_size)
		{
			std::cout << "Buffer update overflow, updateSize[" << updateInfo.offset + updateInfo.size << "], bufferSize[" << m_size << "\n";
			return;
		}

		glNamedBufferSubData(m_id, updateInfo.offset, updateInfo.size, updateInfo.pData);
	}

	
	void Buffer::Map(const mk::BufferMapInfo& mapInfo) const
	{
		if (m_isMutable)
		{
			std::cout << "Failed to map mutable buffer\n";
			return;
		}

		if (mapInfo.offset + mapInfo.length > m_size)
		{
			std::cout << "Buffer map overflow, mapSize[" << mapInfo.offset + mapInfo.length << "], bufferSize[" << m_size << "\n";
			return;
		}

		void* dst = glMapNamedBufferRange(m_id, mapInfo.offset, mapInfo.length, mapInfo.access);

		if (!dst)
		{
			std::cerr << "Failed to map buffer range\n";
			return;
		}

		memcpy(dst, mapInfo.pData, mapInfo.length);

		if (!glUnmapNamedBuffer(m_id))
		{
			std::cerr << "Failed to unmap buffer properly\n";
			return;
		}
	}

}
