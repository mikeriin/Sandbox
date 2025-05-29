#ifndef MK_VERTEX_ARRAY_H
#define MK_VERTEX_ARRAY_H


#include <cstdint>

#include "globals.h"


namespace mk
{

	struct VertexArrayAttributeInfo
	{
		uint32_t index;
		uint32_t size;
		uint32_t type;
		bool isNormalized;
		uint32_t relativeOffset;
	};


	struct VertexArrayCreateInfo
	{
		uint32_t binding;
		uint32_t vertexID;
		uint32_t offset;
		uint32_t stride;
		uint32_t elementID = MK_NO_ELEMENT_BUFFER_ID;
		uint32_t attribCount;
		mk::VertexArrayAttributeInfo* pAttribs;
	};


	class VertexArray
	{
	private:
		uint32_t m_id = 0;
		bool m_hasElement = false;


	public:
		VertexArray() = default;
		~VertexArray();


		bool Create(const mk::VertexArrayCreateInfo& vaInfo);

		void Bind() const;

		inline uint32_t GetID() const { return m_id; }
		inline bool HasElement() const { return m_hasElement; }
	};




}




#endif // !MK_VERTEX_ARRAY_H
