#ifndef MK_WORLD_GENERATOR
#define MK_WORLD_GENERATOR


#include <thread>
#include <stop_token>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

#include <glm.hpp>

#include "thread_safe_queue.h"
#include "voxel_mesh.h"



namespace std
{
	template<>
	struct hash<glm::ivec3>
	{
		size_t operator()(const glm::ivec3& v) const
		{
			size_t h1 = std::hash<int>()(v.x);
			size_t h2 = std::hash<int>()(v.y);
			size_t h3 = std::hash<int>()(v.z);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}



namespace mk
{

	struct VoxelChunkTask
	{
		glm::ivec3 chunkPosition;
		int seed;
	};


	struct VoxelChunkResult
	{
		glm::ivec3 chunkPosition;
		mk::VoxelMeshCreateInfo meshInfo;
	};


	using ChunkMeshes = std::unordered_map<glm::ivec3, std::shared_ptr<mk::VoxelMesh>>;


	class WorldGenerator
	{
	private:
		glm::ivec3 m_cameraChunkPos = { 0, 0, 0 };
		mk::ChunkMeshes m_chunkMeshes;
		std::unordered_set<glm::ivec3> m_pending; // chunk asked
		mk::ThreadSafeQueue<mk::VoxelChunkTask> m_taskQueue;
		mk::ThreadSafeQueue<mk::VoxelChunkResult> m_resultQueue;
		std::vector<std::jthread> m_workerThreads;



	public:
		WorldGenerator();
		~WorldGenerator() = default;


		void RequestChunk(const mk::VoxelChunkTask& task);
		std::optional<mk::VoxelChunkResult> GetGeneratedChunk();

		void UpdateCameraPosition(const glm::vec3& cameraPos);
		void Tick();
		inline const mk::ChunkMeshes& GetChunks() const { return m_chunkMeshes; };


	private:
		void workerLoop(std::stop_token stopToken);
		void scheduleChunks();
		void unloadFarChunks();
	};


}



#endif // !MK_WORLD_GENERATOR
