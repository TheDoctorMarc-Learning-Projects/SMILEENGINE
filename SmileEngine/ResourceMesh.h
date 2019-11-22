#pragma once
#include "Resource.h"
#include "SmileSetup.h"
#include "ComponentMesh.h"
#include "MathGeoLib/include/Geometry/AABB.h" 
#include "parshapes/par_shapes.h"

struct ModelMeshData
{
public:
	uint id_index = 0;
	uint num_index = 0;
	uint* index = nullptr;

	uint num_normals = 0;
	uint id_normals = 0;
	float* normals = nullptr;

	uint id_vertex = 0;
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint id_color = 0;
	uint num_color = 0;
	float* color = nullptr;

	uint id_UVs = 0;
	uint num_UVs = 0;
	float* UVs = nullptr;

	friend class SmileFBX;
	friend class ComponentMesh;
};


class ResourceMesh : public Resource
{
public:

	ResourceMesh(SmileUUID uuid, Resource_Type type, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) {};
	ResourceMesh(SmileUUID uuid, par_shapes_mesh* parshapes, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) { GenerateModelMeshFromParShapes(parshapes); };
	ResourceMesh(SmileUUID uuid, ModelMeshData* model_mesh, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) { this->model_mesh = model_mesh; };
	virtual ~ResourceMesh() {};


	void LoadOnMemory();
	void FreeMemory();
	AABB GetEnclosingAABB(); 
	void GenerateModelMeshFromParShapes(par_shapes_mesh* mesh);

public:
	ModelMeshData* model_mesh = nullptr;


	
};