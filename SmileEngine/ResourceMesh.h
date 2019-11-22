#pragma once
#include "Resource.h"
#include "SmileSetup.h"
#include "ComponentMesh.h"
#include "MathGeoLib/include/Geometry/AABB.h" 

class ResourceMesh : public Resource
{
public:
	enum Mesh_Buffers
	{
		mesh_b_index,
		mesh_b_vertex,
		mesh_b_normals,
		mesh_b_colors,
		mesh_b_UVs,
		max_b,
	};

	ResourceMesh(SmileUUID uuid) : Resource(uuid, Resource_Type::RESOURCE_MESH) {};
	virtual ~ResourceMesh() {};

	void LoadOnMemory();
	void FreeMemory();
	AABB GetEnclosingAABB(); 

public:
	uint buffers[max_b];
	uint buffersSize[max_b];

	uint* index = nullptr;
	float* vertex = nullptr;
	float* normals = nullptr;
	float* colors = nullptr;
	float* UVs = nullptr;

	
	
};