#ifndef MK_BUFFER_H
#define MK_BUFFER_H


#include <cstdint>

#include "globals.h"



namespace mk
{


	struct BufferCreateInfo
	{
		uint32_t size;
		const void* pData;
		bool isMutable;
		uint32_t flags; // immutable
		uint32_t usage; // mutable
		uint32_t binding = MK_NO_BUFFER_BINDING_INDEX;
		uint32_t target;
	};


	struct BufferUpdateInfo
	{
		uint32_t offset;
		uint32_t size;
		const void* pData;
	};


	struct BufferMapInfo
	{
		uint32_t offset;
		uint32_t length;
		uint32_t access;
		const void* pData;
	};



	class Buffer
	{
	private:
		uint32_t m_id = 0;
		uint32_t m_size = 0;
		bool m_isMutable = false;


	public:
		Buffer() = default;
		~Buffer();


		bool Create(const mk::BufferCreateInfo& bufferInfo);

		void Update(const mk::BufferUpdateInfo& updateInfo) const;
		void Map(const mk::BufferMapInfo& mapInfo) const;

		inline uint32_t GetID() const { return m_id; }
	};



}



#endif // !MK_BUFFER_H
