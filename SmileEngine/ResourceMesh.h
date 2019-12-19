#pragma once
#include "Resource.h"
#include "SmileSetup.h"
#include "ComponentMesh.h"
#include "MathGeoLib/include/Geometry/AABB.h" 
#include "parshapes/par_shapes.h"

enum ownMeshType { plane, no_type };

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

	// This is for special draw cases like a plane with procedurally generated points and uvs 
	ownMeshType type = ownMeshType::no_type; 
	float size = 0.f; 

	friend class SmileFBX;
	friend class ComponentMesh;
};

struct ownMeshData // todo, this is actually only worth for a plane
{
public: 
	float size = 0.f;
	std::array<float,8> uvCoords; 
	std::array<float, 8> points; // x,y
	ownMeshType type; 
};

class ResourceMesh : public Resource
{
public:

	ResourceMesh(SmileUUID uuid, Resource_Type type, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) {};
	ResourceMesh(SmileUUID uuid, par_shapes_mesh* parshapes, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) { GenerateModelMeshFromParShapes(parshapes); };
	ResourceMesh(SmileUUID uuid, ownMeshType type, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) { GenerateModelMeshFromOwnType(type); };
	ResourceMesh(SmileUUID uuid, ModelMeshData* model_mesh, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) { this->model_mesh = model_mesh; };
	virtual ~ResourceMesh() {};


	void LoadOnMemory(const char* path = { 0 });
	void FreeMemory();
	AABB GetEnclosingAABB(); 
	void GenerateModelMeshFromParShapes(par_shapes_mesh* mesh);
	void GenerateModelMeshFromOwnType(ownMeshType type);

public:
	ModelMeshData* model_mesh = nullptr;
	ownMeshData* own_mesh = nullptr; 
	
};