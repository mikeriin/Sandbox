#include "voxel_mesh.h"


#include <array>

#include <glad/glad.h>



namespace mk
{



	bool VoxelMesh::Create(mk::VoxelMeshCreateInfo&& voxMeshInfo)
	{
		m_vertices = std::move(voxMeshInfo.vertices);
		m_indices = std::move(voxMeshInfo.indices);

		initGPU();

		if (m_vbo.GetID() || m_ebo.GetID() || !m_vao.GetID()) return false;

		return true;
	}

	bool VoxelMesh::Create(const mk::VoxelMeshCreateInfo& voxMeshInfo)
	{
		m_vertices = voxMeshInfo.vertices;
		m_indices = voxMeshInfo.indices;
		m_aabb = voxMeshInfo.aabb;

		initGPU();

		if (m_vbo.GetID() || m_ebo.GetID() || !m_vao.GetID()) return false;

		return true;
	}

	void VoxelMesh::Draw() const
	{
		glBindVertexArray(m_vao.GetID());
		glDrawElements(GL_TRIANGLES, (int)m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	void VoxelMesh::UpdateGPU() const
	{

	}

	void VoxelMesh::initGPU()
	{
		mk::BufferCreateInfo bufferInfo{};
		bufferInfo.size = static_cast<uint32_t>(m_vertices.size() * sizeof(mk::Voxel));
		bufferInfo.pData = m_vertices.data();
		bufferInfo.isMutable = true;
		bufferInfo.usage = GL_DYNAMIC_DRAW;

		if (!m_vbo.Create(bufferInfo)) return;

		bufferInfo.size = static_cast<uint32_t>(m_indices.size() * sizeof(uint32_t));
		bufferInfo.pData = m_indices.data();

		if (!m_ebo.Create(bufferInfo)) return;

		std::array<mk::VertexArrayAttributeInfo, 2> attribs{};
		attribs[0].index = 0;
		attribs[0].size = 1;
		attribs[0].type = GL_UNSIGNED_INT;
		attribs[0].relativeOffset = offsetof(mk::Voxel, packedA);
		attribs[1].index = 1;
		attribs[1].size = 1;
		attribs[1].type = GL_UNSIGNED_INT;
		attribs[1].relativeOffset = offsetof(mk::Voxel, packedB);

		mk::VertexArrayCreateInfo vaInfo{};
		vaInfo.binding = 0;
		vaInfo.vertexID = m_vbo.GetID();
		vaInfo.offset = 0;
		vaInfo.stride = sizeof(mk::Voxel);
		vaInfo.elementID = m_ebo.GetID();
		vaInfo.attribCount = static_cast<uint32_t>(attribs.size());
		vaInfo.pAttribs = attribs.data();

		m_vao.Create(vaInfo);
	}
}
