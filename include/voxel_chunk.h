#ifndef MK_VOXEL_CHUNK
#define MK_VOXEL_CHUNK


#include <vector>
#include <cstdint>
#include <string>

#include <glm.hpp>

#include "voxel.h"
#include "voxel_mesh.h"



namespace mk
{


	struct VoxelChunkCreateInfo
	{
		int seed;
		glm::ivec3 chunkPosition;
		std::string name;
	};


	struct HeightRange
	{
		int minY;
		int maxY;
	};



	class VoxelChunk
	{
	private:
		int m_seed = 0;
		glm::ivec3 m_chunkPosition = {};
		std::vector<bool> m_datas = {};
		bool m_empty = true;


	public:
		VoxelChunk() = default;
		~VoxelChunk() = default;

		
		bool Create(const mk::VoxelChunkCreateInfo voxChunkInfo);


		inline size_t GetIndex(uint8_t x, uint8_t y, uint8_t z) const
			{ return x * MK_VOXEL_CHUNK_BOUND * MK_VOXEL_CHUNK_BOUND + z * MK_VOXEL_CHUNK_BOUND + y; }
		inline size_t GetIndex(const glm::ivec3& pos) const { return GetIndex(pos.x, pos.y, pos.z); }
		inline const glm::ivec3& GetPosition() const { return m_chunkPosition; }
		inline bool IsEmpty() const { return m_empty; }


		mk::VoxelMeshCreateInfo GenerateMesh() const;

	};




}




#endif // !MK_VOXEL_CHUNK
