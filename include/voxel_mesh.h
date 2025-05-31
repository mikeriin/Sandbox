#ifndef MK_MESH_H
#define MK_MESH_H


#include <cstdint>
#include <vector>

#include <glm.hpp>

#include "buffer.h"
#include "vertex_array.h"
#include "voxel.h"
#include "aabb.h"



namespace mk
{


	struct VoxelMeshCreateInfo
	{
		std::vector<mk::Voxel> vertices;
		std::vector<uint32_t> indices;
		mk::AABB aabb;
	};



	class VoxelMesh
	{
	private:
		std::vector<mk::Voxel> m_vertices = {};
		std::vector<uint32_t> m_indices = {};
		mk::AABB m_aabb = {};
		mk::Buffer m_vbo = {};
		mk::Buffer m_ebo = {};
		mk::VertexArray m_vao = {};
		bool m_isDirty = false;


	public:
		VoxelMesh() = default;
		~VoxelMesh() = default;


		bool Create(mk::VoxelMeshCreateInfo&& voxMeshInfo);
		bool Create(const mk::VoxelMeshCreateInfo& voxMeshInfo);


		void Draw() const;
		void UpdateGPU() const;

		
		inline std::vector<mk::Voxel>& GetVertices() { return m_vertices; }
		inline const std::vector<mk::Voxel>& GetVertices() const { return m_vertices; }

		inline std::vector<uint32_t>& GetIndices() { return m_indices; }
		inline const std::vector<uint32_t>& GetIndices() const { return m_indices; }

		inline const mk::AABB& GetAABB() const { return m_aabb; }

	private:
		void initGPU();
	};


}




#endif // !MK_MESH_H
