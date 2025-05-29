#include "voxel_chunk.h"


#include <iostream>
#include <algorithm>

#include <FastNoiseLite.h>


namespace mk
{





	bool VoxelChunk::Create(const mk::VoxelChunkCreateInfo voxChunkInfo)
	{
		m_seed = voxChunkInfo.seed;
		m_chunkPosition = voxChunkInfo.chunkPosition;

		m_datas.resize(MK_VOXEL_CHUNK_BOUND * MK_VOXEL_CHUNK_BOUND * MK_VOXEL_CHUNK_BOUND, false);

		FastNoiseLite heightNoiseFunc;
		heightNoiseFunc.SetSeed(m_seed);
		heightNoiseFunc.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		heightNoiseFunc.SetFrequency(0.002f);

		FastNoiseLite veinNoiseFunc;
		veinNoiseFunc.SetSeed(m_seed + 42);
		veinNoiseFunc.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		veinNoiseFunc.SetFrequency(0.002f);
		veinNoiseFunc.SetFractalType(FastNoiseLite::FractalType_PingPong);
		veinNoiseFunc.SetFractalOctaves(1);

		FastNoiseLite caveNoiseFunc;
		caveNoiseFunc.SetSeed(m_seed + 84);
		caveNoiseFunc.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		caveNoiseFunc.SetFrequency(0.01f);
		caveNoiseFunc.SetFractalType(FastNoiseLite::FractalType_FBm);
		caveNoiseFunc.SetFractalOctaves(3);
		caveNoiseFunc.SetFractalLacunarity(2.05f);
		caveNoiseFunc.SetFractalGain(0.6f);

		glm::ivec3 worldPosition = m_chunkPosition * (int)MK_VOXEL_CHUNK_SIZE;

		/*for (uint8_t x = 0; x < MK_VOXEL_CHUNK_BOUND; x++)
		{
			for (uint8_t z = 0; z < MK_VOXEL_CHUNK_BOUND; z++)
			{
				for (uint8_t y = 0; y < MK_VOXEL_CHUNK_BOUND; y++)
				{
					float noise = heightNoiseFunc.GetNoise(
						worldPosition.x + (float)x,
						worldPosition.y + (float)y,
						worldPosition.z + (float)z);

					if (noise > 0.5f)
					{
						if (m_empty) m_empty = false;
						m_datas[GetIndex(x, y, z)] = true;
					}
				}
			}
		}*/

		for (uint8_t x = 0; x < MK_VOXEL_CHUNK_BOUND; x++)
		{
			for (uint8_t z = 0; z < MK_VOXEL_CHUNK_BOUND; z++)
			{
				float persistence = 0.5f;
				float lacunarity = 2.05f;
				float frequency = 1.0f;
				float amplitude = 2.0f;
				float total = 0.0f;
				float maxAmplitude = 0.0f;
				int   octaves = 5;

				for (int i = 0; i < octaves; ++i) {
					float nx = (worldPosition.x + x) * frequency;
					float nz = (worldPosition.z + z) * frequency;
					float noise = heightNoiseFunc.GetNoise(nx, nz);
					noise = (noise + 1.0f) * 0.5f;

					total += noise * amplitude;
					maxAmplitude += amplitude;

					amplitude *= persistence;
					frequency *= lacunarity;
				}
				total /= maxAmplitude;
				total = std::pow(total, 4.0f);

				int surfaceHeight = (int)(std::floor(total * MK_VOXEL_CHUNK_MAX_GEN_HEIGHT) + MK_VOXEL_CHUNK_PLAIN_HEIGHT);
				int localHeight = surfaceHeight - m_chunkPosition.y * (int)MK_VOXEL_CHUNK_SIZE;

				int end = std::clamp(localHeight, 0, (int)MK_VOXEL_CHUNK_BOUND);

				for (uint8_t y = 0; y < end; y++)
				{
					float depth = std::max(0.0f, -((float)worldPosition.y + y));
					constexpr float MAX_DEPTH = 32.0f;
					float heightRatio = std::clamp(depth / MAX_DEPTH, 0.0f, 1.0f);

					float minThreshold = 1.0f;
					float maxThreshold = 0.5f;
					float smoothRatio = heightRatio * heightRatio * (3.0f - 2.0f * heightRatio);
					float veinThreshold = std::lerp(minThreshold, maxThreshold, smoothRatio);

					float nx = (float)worldPosition.x + x;
					float ny = (float)worldPosition.y + y;
					float nz = (float)worldPosition.z + z;

					float veinNoise = veinNoiseFunc.GetNoise(nx, ny, nz);
					float veinNoise01 = (veinNoise + 0.5f) * 0.5f;

					float caveNoise = caveNoiseFunc.GetNoise(nx, ny, nz);
					float caveNoise01 = (caveNoise + 0.5f) * 0.5f;

					float finalCave = glm::mix(veinNoise01, caveNoise01, 0.7f);

					if (finalCave > veinThreshold) continue;

					if (m_empty) m_empty = false;
					m_datas[GetIndex(x, y, z)] = true;
				}
			}
		}

		return true;
	}


	mk::VoxelMeshCreateInfo VoxelChunk::GenerateMesh() const
	{
		auto isBlockSolid = [&](const glm::ivec3& pos) -> bool {
			if (
				pos.x < 0 || pos.x >= MK_VOXEL_CHUNK_BOUND 
				|| pos.y < 0 || pos.y >= MK_VOXEL_CHUNK_BOUND 
				|| pos.z < 0 || pos.z >= MK_VOXEL_CHUNK_BOUND
			) return false;

			return m_datas[GetIndex(pos)];
			};


		std::vector<mk::Voxel> vertices;
		std::vector<uint32_t> indices;
		


		for (int x = 0; x < MK_VOXEL_CHUNK_BOUND; x++)
		{
			for (int z = 0; z < MK_VOXEL_CHUNK_BOUND; z++)
			{
				for (int y = 0; y < MK_VOXEL_CHUNK_BOUND; y++)
				{

					if (x == 0 || x > MK_VOXEL_CHUNK_SIZE
						|| y == 0 || y > MK_VOXEL_CHUNK_SIZE
						|| z == 0 || z > MK_VOXEL_CHUNK_SIZE) continue;

					glm::ivec3 boundPos(x, y, z);
					if (!isBlockSolid(boundPos)) continue;
					glm::ivec3 realPos = boundPos - glm::ivec3(1);


					for (uint8_t normal = 0; normal < 6; normal++)
					{
						glm::ivec3 neighboorPos = boundPos + mk::NORMALS[normal];
						bool neighboor = isBlockSolid(neighboorPos);

						if (neighboor) continue;

						uint32_t base = (uint32_t)vertices.size();
						mk::VoxelFace face = mk::CreateFace(realPos, (mk::FaceDirection)normal, glm::ivec2(1));
						int aos[4]{};

						// 4 corners
						for (uint8_t index = 0; index < 4; index++)
						{
							mk::AOCornerOffsets offsets = mk::GetAOCornerOffsets((mk::FaceDirection)normal, index);

							glm::ivec3 side1Pos = boundPos + offsets[0];
							int side1 = isBlockSolid(side1Pos);
							glm::ivec3 side2Pos = boundPos + offsets[1];
							int side2 = isBlockSolid(side2Pos);
							glm::ivec3 cornerPos = boundPos + offsets[2];
							int corner = isBlockSolid(cornerPos);

							uint8_t u = mk::GetVoxelU(face[index]);
							uint8_t v = mk::GetVoxelV(face[index]);
							uint8_t ao = 0;

							if (side1 && side2) ao = 3;
							else ao += side1 + side2 + corner;

							face[index].packedB = mk::PackUVExtentAndAO(u, v, ao);

							aos[index] = ao;
						}

						std::array<uint32_t, 6> faceIndices =
						{
							base,			base + 3,	base + 1,
							base + 1,	base + 3,	base + 2
						};

						if (aos[3] + aos[1] > aos[0] + aos[2])
						{
							std::swap(face[0], face[3]);
							std::swap(face[1], face[2]);

							std::swap(faceIndices[1], faceIndices[2]);
							std::swap(faceIndices[3], faceIndices[4]);
						}

						vertices.insert(vertices.end(), face.begin(), face.end());

						indices.insert(indices.end(), faceIndices.begin(), faceIndices.end());
					}
				}
			}
		}



		mk::VoxelMeshCreateInfo voxMeshInfo{};
		voxMeshInfo.vertices = vertices;
		voxMeshInfo.indices = indices;

		return voxMeshInfo;
	}


}