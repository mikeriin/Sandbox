#include "world_generator.h"


#include <functional>
#include <cmath>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>

#include "globals.h"
#include "voxel_chunk.h"



namespace mk
{




    WorldGenerator::WorldGenerator()
    {
      for (int i = 0; i < 2; i++)
      {
        m_workerThreads.emplace_back(std::bind_front(&WorldGenerator::workerLoop, this));
      }
    }


    void WorldGenerator::RequestChunk(const mk::VoxelChunkTask& task)
    {
      m_taskQueue.Push(task);
    }


    std::optional<mk::VoxelChunkResult> WorldGenerator::GetGeneratedChunk()
    {
      return m_resultQueue.TryPop();
    }

    void WorldGenerator::UpdateCameraPosition(const glm::vec3& cameraPos)
    {
      m_cameraChunkPos = glm::floor(cameraPos / (float)MK_VOXEL_CHUNK_SIZE);
    }

    void WorldGenerator::Tick()
    {
      while (auto result = m_resultQueue.TryPop())
      {
        auto mesh = std::make_shared<mk::VoxelMesh>();
        mesh->Create(result->meshInfo);

        m_chunkMeshes[result->chunkPosition] = std::move(mesh);
        m_pending.erase(result->chunkPosition);
      }

      unloadFarChunks();
      scheduleChunks();
    }


    void WorldGenerator::workerLoop(std::stop_token stopToken)
    {
      while (!stopToken.stop_requested())
      {
        auto task = m_taskQueue.PopWait(stopToken);
        if (!task) break;

        mk::VoxelChunkCreateInfo voxChunkInfo{};
        voxChunkInfo.chunkPosition = task->chunkPosition;
        voxChunkInfo.seed = task->seed;

        mk::VoxelChunk voxChunk;
        voxChunk.Create(voxChunkInfo);

        if (voxChunk.IsEmpty()) continue;

        mk::VoxelMeshCreateInfo voxMeshInfo = voxChunk.GenerateMesh();

        mk::VoxelChunkResult voxChunkResult{};
        voxChunkResult.chunkPosition = task->chunkPosition;
        voxChunkResult.meshInfo = std::move(voxMeshInfo);

        m_resultQueue.Push(voxChunkResult);
      }
    }


    void WorldGenerator::scheduleChunks()
    {
      std::vector<glm::ivec3> positions;

      for (int dx = -MK_VOXEL_CHUNK_RENDER_DIST; dx < MK_VOXEL_CHUNK_RENDER_DIST; dx++)
      {
        for (int dy = -MK_VOXEL_CHUNK_RENDER_DIST; dy < MK_VOXEL_CHUNK_RENDER_DIST; dy++)
        {
          for (int dz = -MK_VOXEL_CHUNK_RENDER_DIST; dz < MK_VOXEL_CHUNK_RENDER_DIST; dz++)
          {
            if (glm::length(glm::vec3(dx, dy, dz)) > (float)MK_VOXEL_CHUNK_RENDER_DIST) continue;

            glm::ivec3 pos(
              m_cameraChunkPos.x + dx,
              m_cameraChunkPos.y + dy,
              m_cameraChunkPos.z + dz
            );
            positions.push_back(pos);
          }
        }
      }

      // thanks gpt
      std::sort(positions.begin(), positions.end(), [&](const glm::ivec3& a, const glm::ivec3& b) {
        int dxA = a.x - m_cameraChunkPos.x;
        int dyA = a.y - m_cameraChunkPos.y;
        int dzA = a.z - m_cameraChunkPos.z;

        int dxB = b.x - m_cameraChunkPos.x;
        int dyB = b.y - m_cameraChunkPos.y;
        int dzB = b.z - m_cameraChunkPos.z;

        int distSqA = dxA * dxA + dyA * dyA + dzA * dzA;
        int distSqB = dxB * dxB + dyB * dyB + dzB * dzB;

        return distSqA < distSqB;
        });

      for (const auto& pos : positions)
      {
        if (m_chunkMeshes.contains(pos) || m_pending.contains(pos)) continue;

        m_taskQueue.Push(mk::VoxelChunkTask{ pos, mk::seed });
        m_pending.insert(pos);
      }
    }


    void WorldGenerator::unloadFarChunks()
    {
      std::vector<glm::ivec3> toRemove;

      for (const auto& [pos, mesh] : m_chunkMeshes)
      {
        int dx = std::abs(pos.x - m_cameraChunkPos.x);
        int dy = std::abs(pos.y - m_cameraChunkPos.y);
        int dz = std::abs(pos.z - m_cameraChunkPos.z);

        if (glm::length2(glm::vec3(dx, dy, dz)) > MK_VOXEL_CHUNK_RENDER_DIST * MK_VOXEL_CHUNK_RENDER_DIST)
          toRemove.push_back(pos);
      }

      for (const auto& pos : toRemove) m_chunkMeshes.erase(pos);
    }

}