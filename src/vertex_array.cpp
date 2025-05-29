#include "vertex_array.h"


#include <glad/glad.h>




namespace mk
{
	VertexArray::~VertexArray()
	{
		if (m_id) glDeleteVertexArrays(1, &m_id);
	}


	bool VertexArray::Create(const mk::VertexArrayCreateInfo& vaInfo)
	{
		glCreateVertexArrays(1, &m_id);
		glVertexArrayVertexBuffer(m_id, vaInfo.binding, vaInfo.vertexID, vaInfo.offset, vaInfo.stride);

		if (vaInfo.elementID != MK_NO_ELEMENT_BUFFER_ID)
		{
			glVertexArrayElementBuffer(m_id, vaInfo.elementID);
			m_hasElement = true;
		}

		
		for (size_t i = 0; i < vaInfo.attribCount; i++)
		{
			const auto& attrib = vaInfo.pAttribs[i];

			switch (attrib.type)
			{
			case GL_FLOAT:
				glVertexArrayAttribFormat(m_id, attrib.index, attrib.size, attrib.type, attrib.isNormalized, attrib.relativeOffset);
				break;

			case GL_UNSIGNED_INT:
			default:
				glVertexArrayAttribIFormat(m_id, attrib.index, attrib.size, attrib.type, attrib.relativeOffset);
				break;
			}

			glVertexArrayAttribBinding(m_id, attrib.index, vaInfo.binding);
			glEnableVertexArrayAttrib(m_id, attrib.index);
		}


		return true;
	}


	void VertexArray::Bind() const
	{
		glBindVertexArray(m_id);
	}
}