#version 460 core

#define X_SHIFT			15u
#define Y_SHIFT			9u
#define Z_SHIFT			3u
#define NORM_SHIFT	0u
#define TYPE_SHIFT	15u
#define U_SHIFT			9u
#define V_SHIFT			3u
#define AO_SHIFT		0u

#define MASK_8_BITS 0xFFu
#define MASK_6_BITS 0x3Fu
#define MASK_3_BITS 0x7u

layout(location = 0) in uint a_VoxelPackedA;
layout(location = 1) in uint a_VoxelPackedB;

layout(location = 0) out float outAO;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPos;
layout(location = 3) out vec2 outUV;
layout(location = 4) smooth out vec3 outViewPosition;
layout(location = 5) flat out int outType;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
	mat4 u_ViewProjection;
};
	

uniform vec3 u_ChunkPosition;


// packedA
vec3 GetVertexPosition(uint voxel)
{
	return vec3(
		float((voxel >> X_SHIFT) & MASK_6_BITS),
		float((voxel >> Y_SHIFT) & MASK_6_BITS),
		float((voxel >> Z_SHIFT) & MASK_6_BITS)
	);
}

vec3 Normals[6] = vec3[6](
	vec3( 0,  0,  1), // +Z
	vec3( 0,  0, -1),	// -Z
	vec3( 1,  0,  0), // +X
	vec3(-1,  0,  0), // -X
	vec3( 0,  1,  0), // +Y
	vec3( 0, -1,  0)  // -Y
);

vec3 GetVertexNormal(uint voxel)
{
	return Normals[(voxel >> NORM_SHIFT) & MASK_3_BITS];
}

// packedB
uint GetVertexType(uint voxel)
{
	return (voxel >> TYPE_SHIFT) & MASK_8_BITS;
}

vec2 GetVertexUV(uint voxel)
{
	return vec2(
		float((voxel >> U_SHIFT) & MASK_6_BITS),
		float((voxel >> V_SHIFT) & MASK_6_BITS)
	);
}

uint GetVertexAO(uint voxel)
{
	return (voxel >> AO_SHIFT) & MASK_3_BITS;
}




mat4 model = mat4(1.0);


void main()
{
	vec3 position = GetVertexPosition(a_VoxelPackedA);
	if (length(u_ChunkPosition) != 0)
		position += u_ChunkPosition * 32;
		
	vec4 viewPos4 = u_View * model * vec4(position, 1.0);
	outViewPosition = viewPos4.xyz;
	gl_Position = u_Projection * viewPos4;

	outAO = GetVertexAO(a_VoxelPackedB) * 0.33;
	outNormal = GetVertexNormal(a_VoxelPackedA);
	outPos = GetVertexPosition(a_VoxelPackedA);
	outUV = GetVertexUV(a_VoxelPackedB);
	outType = int(GetVertexType(a_VoxelPackedB));
}