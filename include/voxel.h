#ifndef MK_VOXEL_H
#define MK_VOXEL_H


#include <cstdint>
#include <unordered_map>
#include <array>

#include <glm.hpp>

#include "globals.h"



namespace mk
{


	struct Voxel
	{
		uint32_t packedA; // position + normal
		uint32_t packedB; // uv + ao
	};


	inline uint32_t PackPositionAndNormal(uint8_t x, uint8_t y, uint8_t z, uint8_t norm);
	inline uint32_t PackUVExtentAndAO(uint8_t u, uint8_t v, uint8_t ao);
	inline uint8_t GetVoxelX(const mk::Voxel& vox);
	inline uint8_t GetVoxelY(const mk::Voxel& vox);
	inline uint8_t GetVoxelZ(const mk::Voxel& vox);
	inline uint8_t GetVoxelNormal(const mk::Voxel& vox);
	inline uint8_t GetVoxelU(const mk::Voxel& vox);
	inline uint8_t GetVoxelV(const mk::Voxel& vox);
	inline uint8_t GetVoxelAO(const mk::Voxel& vox);



	using AOCornerOffsets = std::array<glm::ivec3, 3>;
	using AOFaceOffsets = std::array<mk::AOCornerOffsets, 4>;


	enum FaceDirection : uint8_t
	{
		Front = 0,
		Back,
		Right,
		Left,
		Top,
		Bottom
	};


	const glm::ivec3 NORMALS[6] =
	{
		glm::ivec3( 0, 0, 1),	// front
		glm::ivec3( 0, 0,-1),	// back
		glm::ivec3( 1, 0, 0),	// right
		glm::ivec3(-1, 0, 0),	// left
		glm::ivec3( 0, 1, 0),	// top
		glm::ivec3( 0,-1, 0)	// bottom
	};


	const std::unordered_map<uint8_t, AOFaceOffsets> AO_OFFSETS =
	{
		// FRONT (+Z)
		{ mk::FaceDirection::Front, {
				AOCornerOffsets{{ glm::ivec3(-1, 0, 1), glm::ivec3(0, 1, 1), glm::ivec3(-1, 1, 1) }},
				AOCornerOffsets{{ glm::ivec3(0, 1, 1), glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 1) }},
				AOCornerOffsets{{ glm::ivec3(1, 0, 1), glm::ivec3(0,-1, 1), glm::ivec3(1,-1, 1) }},
				AOCornerOffsets{{ glm::ivec3(0,-1, 1), glm::ivec3(-1, 0, 1), glm::ivec3(-1,-1, 1) }}
		}},

		// BACK (–Z)
		{ mk::FaceDirection::Back, {
				AOCornerOffsets{{ glm::ivec3(1, 0,-1), glm::ivec3(0, 1,-1), glm::ivec3(1, 1,-1) }},
				AOCornerOffsets{{ glm::ivec3(0, 1,-1), glm::ivec3(-1, 0,-1), glm::ivec3(-1, 1,-1) }},
				AOCornerOffsets{{ glm::ivec3(-1, 0,-1), glm::ivec3(0,-1,-1), glm::ivec3(-1,-1,-1) }},
				AOCornerOffsets{{ glm::ivec3(0,-1,-1), glm::ivec3(1, 0,-1), glm::ivec3(1,-1,-1) }}
		}},

		// RIGHT (+X)
		{ mk::FaceDirection::Right, {
				AOCornerOffsets{{ glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 0), glm::ivec3(1, 1, 1) }},
				AOCornerOffsets{{ glm::ivec3(1, 1, 0), glm::ivec3(1, 0,-1), glm::ivec3(1, 1,-1) }},
				AOCornerOffsets{{ glm::ivec3(1, 0,-1), glm::ivec3(1,-1, 0), glm::ivec3(1,-1,-1) }},
				AOCornerOffsets{{ glm::ivec3(1,-1, 0), glm::ivec3(1, 0, 1), glm::ivec3(1,-1, 1) }}
		}},

		// LEFT (–X)
		{ mk::FaceDirection::Left, {
				AOCornerOffsets{{ glm::ivec3(-1, 0,-1), glm::ivec3(-1, 1, 0), glm::ivec3(-1, 1,-1) }},
				AOCornerOffsets{{ glm::ivec3(-1, 1, 0), glm::ivec3(-1, 0, 1), glm::ivec3(-1, 1, 1) }},
				AOCornerOffsets{{ glm::ivec3(-1, 0, 1), glm::ivec3(-1,-1, 0), glm::ivec3(-1,-1, 1) }},
				AOCornerOffsets{{ glm::ivec3(-1,-1, 0), glm::ivec3(-1, 0,-1), glm::ivec3(-1,-1,-1) }}
		}},

		// TOP (+Y)
		{ mk::FaceDirection::Top, {
				AOCornerOffsets{{ glm::ivec3(-1, 1, 0), glm::ivec3(0, 1,-1), glm::ivec3(-1, 1,-1) }},
				AOCornerOffsets{{ glm::ivec3(0, 1,-1), glm::ivec3(1, 1, 0), glm::ivec3(1, 1,-1) }},
				AOCornerOffsets{{ glm::ivec3(1, 1, 0), glm::ivec3(0, 1, 1), glm::ivec3(1, 1, 1) }},
				AOCornerOffsets{{ glm::ivec3(0, 1, 1), glm::ivec3(-1, 1, 0), glm::ivec3(-1, 1, 1) }}
		}},

		// BOTTOM (–Y)
		{ mk::FaceDirection::Bottom, {
				AOCornerOffsets{{ glm::ivec3(-1,-1, 0), glm::ivec3(0,-1, 1), glm::ivec3(-1,-1, 1) }},
				AOCornerOffsets{{ glm::ivec3(0,-1, 1), glm::ivec3(1,-1, 0), glm::ivec3(1,-1, 1) }},
				AOCornerOffsets{{ glm::ivec3(1,-1, 0), glm::ivec3(0,-1,-1), glm::ivec3(1,-1,-1) }},
				AOCornerOffsets{{ glm::ivec3(0,-1,-1), glm::ivec3(-1,-1, 0), glm::ivec3(-1,-1,-1) }}
		}},
	};

	inline const mk::AOCornerOffsets& GetAOCornerOffsets(uint8_t face, uint8_t index);


	using VoxelFace = std::array<mk::Voxel, 4>;


	inline VoxelFace CreateFace(const glm::ivec3& pos, mk::FaceDirection face, const glm::ivec2 extent);
}


uint32_t mk::PackPositionAndNormal(uint8_t x, uint8_t y, uint8_t z, uint8_t norm)
{
	return (x & MK_MASK_6_BITS) << MK_VOXEL_X_SHIFT |
		(y & MK_MASK_6_BITS) << MK_VOXEL_Y_SHIFT |
		(z & MK_MASK_6_BITS) << MK_VOXEL_Z_SHIFT |
		(norm & MK_MASK_3_BITS) << MK_VOXEL_NORMAL_SHIFT;
}

uint32_t mk::PackUVExtentAndAO(uint8_t u, uint8_t v, uint8_t ao)
{
	return (u & MK_MASK_6_BITS) << MK_VOXEL_U_SHIFT |
		(v & MK_MASK_6_BITS) << MK_VOXEL_V_SHIFT |
		(ao & MK_MASK_3_BITS) << MK_VOXEL_AO_SHIFT;
}

uint8_t mk::GetVoxelX(const mk::Voxel& vox)
{
	return (vox.packedA >> MK_VOXEL_X_SHIFT) & MK_MASK_6_BITS;
}

uint8_t mk::GetVoxelY(const mk::Voxel& vox)
{
	return (vox.packedA >> MK_VOXEL_Y_SHIFT) & MK_MASK_6_BITS;
}

uint8_t mk::GetVoxelZ(const mk::Voxel& vox)
{
	return (vox.packedA >> MK_VOXEL_Z_SHIFT) & MK_MASK_6_BITS;
}

uint8_t mk::GetVoxelNormal(const mk::Voxel& vox)
{
	return (vox.packedA >> MK_VOXEL_NORMAL_SHIFT) & MK_MASK_3_BITS;
}

uint8_t mk::GetVoxelU(const mk::Voxel& vox)
{
	return (vox.packedB >> MK_VOXEL_U_SHIFT) & MK_MASK_6_BITS;
}

uint8_t mk::GetVoxelV(const mk::Voxel& vox)
{
	return (vox.packedB >> MK_VOXEL_V_SHIFT) & MK_MASK_6_BITS;
}

uint8_t mk::GetVoxelAO(const mk::Voxel& vox)
{
	return (vox.packedB >> MK_VOXEL_AO_SHIFT) & MK_MASK_3_BITS;
}

const mk::AOCornerOffsets& mk::GetAOCornerOffsets(uint8_t face, uint8_t index)
{
	return mk::AO_OFFSETS.at(face).at(index);
}

mk::VoxelFace mk::CreateFace(const glm::ivec3& pos, mk::FaceDirection norm, const glm::ivec2 extent)
{
	mk::VoxelFace face;

	switch (norm)
	{
	case mk::Front:
	default:
		face[0] = { PackPositionAndNormal(pos.x						, pos.y + extent.y, pos.z + 1	, norm), PackUVExtentAndAO(0				, extent.y, 0) };
		face[1] = { PackPositionAndNormal(pos.x + extent.x, pos.y + extent.y, pos.z + 1	, norm), PackUVExtentAndAO(extent.x	, extent.y, 0) };
		face[2] = { PackPositionAndNormal(pos.x + extent.x, pos.y						, pos.z + 1	, norm), PackUVExtentAndAO(extent.x	, 0				, 0) };
		face[3] = { PackPositionAndNormal(pos.x						, pos.y						, pos.z + 1	, norm), PackUVExtentAndAO(0				, 0				, 0) };
		break;
	case mk::Back:
		face[0] = { PackPositionAndNormal(pos.x + extent.x, pos.y + extent.y, pos.z			, norm), PackUVExtentAndAO(0				, extent.y, 0) };
		face[1] = { PackPositionAndNormal(pos.x						, pos.y + extent.y, pos.z			, norm), PackUVExtentAndAO(extent.x	, extent.y, 0) };
		face[2] = { PackPositionAndNormal(pos.x						, pos.y						, pos.z			, norm), PackUVExtentAndAO(extent.x	, 0				, 0) };
		face[3] = { PackPositionAndNormal(pos.x + extent.x, pos.y						, pos.z			, norm), PackUVExtentAndAO(0				, 0				, 0) };
		break;
	case mk::Right:
		face[0] = { PackPositionAndNormal(pos.x + 1	, pos.y + extent.y, pos.z + extent.x, norm), PackUVExtentAndAO(0				, extent.y, 0) };
		face[1] = { PackPositionAndNormal(pos.x + 1	, pos.y + extent.y, pos.z						, norm), PackUVExtentAndAO(extent.x	, extent.y, 0) };
		face[2] = { PackPositionAndNormal(pos.x + 1	, pos.y						, pos.z						, norm), PackUVExtentAndAO(extent.x	, 0				, 0) };
		face[3] = { PackPositionAndNormal(pos.x + 1	, pos.y						, pos.z + extent.x, norm), PackUVExtentAndAO(0				, 0				, 0) };
		break;
	case mk::Left:
		face[0] = { PackPositionAndNormal(pos.x			, pos.y + extent.y, pos.z						, norm), PackUVExtentAndAO(0				, extent.y, 0) };
		face[1] = { PackPositionAndNormal(pos.x			, pos.y + extent.y, pos.z + extent.x, norm), PackUVExtentAndAO(extent.x	, extent.y, 0) };
		face[2] = { PackPositionAndNormal(pos.x			, pos.y						, pos.z + extent.x, norm), PackUVExtentAndAO(extent.x	, 0				, 0) };
		face[3] = { PackPositionAndNormal(pos.x			, pos.y						, pos.z						, norm), PackUVExtentAndAO(0				, 0				, 0) };
		break;
	case mk::Top:
		face[0] = { PackPositionAndNormal(pos.x						, pos.y + 1	, pos.z						, norm), PackUVExtentAndAO(0				, extent.y, 0) };
		face[1] = { PackPositionAndNormal(pos.x + extent.x, pos.y + 1	, pos.z						, norm), PackUVExtentAndAO(extent.x	, extent.y, 0) };
		face[2] = { PackPositionAndNormal(pos.x + extent.x, pos.y + 1	, pos.z + extent.y, norm), PackUVExtentAndAO(extent.x	, 0				, 0) };
		face[3] = { PackPositionAndNormal(pos.x						, pos.y + 1	, pos.z + extent.y, norm), PackUVExtentAndAO(0				, 0				, 0) };
		break;
	case mk::Bottom:
		face[0] = { PackPositionAndNormal(pos.x						, pos.y			, pos.z + extent.y, norm), PackUVExtentAndAO(0				, extent.y, 0) };
		face[1] = { PackPositionAndNormal(pos.x + extent.x, pos.y			, pos.z + extent.y, norm), PackUVExtentAndAO(extent.x	, extent.y, 0) };
		face[2] = { PackPositionAndNormal(pos.x + extent.x, pos.y			, pos.z						, norm), PackUVExtentAndAO(extent.x	, 0				, 0) };
		face[3] = { PackPositionAndNormal(pos.x						, pos.y			, pos.z						, norm), PackUVExtentAndAO(0				, 0				, 0) };
		break;
	}

	return face;
}



#endif // !MK_VOXEL_H
