#ifndef MK_GLOBALS_H
#define MK_GLOBALS_H


#include <cstdint>



constexpr auto MK_GFXAPI_OPENGL											= 0x00000001;
constexpr auto MK_GFXAPI_VULKAN											= 0x00000002;
constexpr auto MK_NO_BUFFER_BINDING_INDEX						= 0x00099999;
constexpr auto MK_NO_ELEMENT_BUFFER_ID							= 0x00099999;

constexpr uint32_t MK_MASK_8_BITS										= 0xFF;
constexpr uint32_t MK_MASK_6_BITS										= 0x3F;
constexpr uint32_t MK_MASK_3_BITS										= 0x7;
constexpr uint32_t MK_VOXEL_X_SHIFT									= 15;
constexpr uint32_t MK_VOXEL_Y_SHIFT									= 9;
constexpr uint32_t MK_VOXEL_Z_SHIFT									= 3;
constexpr uint32_t MK_VOXEL_NORMAL_SHIFT						= 0;
constexpr uint32_t MK_VOXEL_BLOCK_TYPE_SHIFT				= 15;
constexpr uint32_t MK_VOXEL_U_SHIFT									= 9;
constexpr uint32_t MK_VOXEL_V_SHIFT									= 3;
constexpr uint32_t MK_VOXEL_AO_SHIFT								= 0;

constexpr float MK_CAMERA_SENSITIVITY_X							= 0.45f;
constexpr float MK_CAMERA_SENSITIVITY_Y							= 0.4f;
constexpr float MK_CAMERA_SPEED											= 50.0f;
constexpr float MK_CAMERA_ROTATION_SMOOTHING_SPEED	= 40.0f;
constexpr auto MK_CAMERA_MODE_FREE_LOOK							= 0x00000003;
constexpr auto MK_CAMERA_MODE_CLASSIC								= 0x00000004;

constexpr uint8_t MK_VOXEL_CHUNK_BOUND							= 34;
constexpr uint8_t MK_VOXEL_CHUNK_SIZE								= 32;
constexpr uint8_t MK_VOXEL_CHUNK_HALF_SIZE					= 16;
constexpr uint8_t MK_VOXEL_CHUNK_PLAIN_HEIGHT				= 0;
constexpr uint16_t MK_VOXEL_CHUNK_MAX_GEN_HEIGHT		= 256;
constexpr uint8_t	MK_VOXEL_CHUNK_RENDER_DIST				= 10;
constexpr uint8_t	MK_VOXEL_CHUNK_RENDER_DIST_Y			= 10;

constexpr int MK_TEXTURE_DEFAULT_CHANNELS						= -1;



namespace mk
{

	extern int gfxApi;
	extern int seed;

}





#endif // !MK_GLOBALS_H
