#pragma once
#include "Resource.h"
#include "SmileSetup.h"
#include "ComponentMesh.h"
#include "MathGeoLib/include/Geometry/AABB.h" 
#include "parshapes/par_shapes.h"
#include "variant"

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

// This should handle more than just the plane type
struct ownMeshData
{
public:
	float size = 0.f;
	std::array<float, 8> initialUvCoords;
	std::array<float, 8> uvCoords;
	std::array<float, 8> points;
	std::array<float, 12> points3D;
	ownMeshType type;

	void ResetUvs() { uvCoords = initialUvCoords; };
};

class ResourceMesh : public Resource
{
public:

	ResourceMesh(SmileUUID uuid, Resource_Type type, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) {};
	ResourceMesh(SmileUUID uuid, par_shapes_mesh* parshapes, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) { GenerateModelMeshFromParShapes(parshapes); };
	ResourceMesh(SmileUUID uuid, ownMeshType type, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) {};
	ResourceMesh(SmileUUID uuid, ModelMeshData* model_mesh, std::string path) : Resource(uuid, Resource_Type::RESOURCE_MESH, path) { this->model_mesh = model_mesh; };
	virtual ~ResourceMesh() {};

	// Create stuff
	void LoadOnMemory(const char* path = { 0 });
	void FreeMemory();
	AABB GetEnclosingAABB(); 
	void GenerateModelMeshFromParShapes(par_shapes_mesh* mesh);
	virtual void GenerateOwnMeshData() {};

	// getters
	std::variant<ModelMeshData*, ownMeshData*> GetMeshData()
	{
		if (model_mesh)
			return model_mesh;
		else if (own_mesh)
			return own_mesh; 
		return (ModelMeshData*)nullptr;
	};

	float* GetVertexBuffer() { return (GetMeshData().index() == 0) ? model_mesh->vertex : own_mesh->points3D.data(); };

	uint GetNumVertex() { return (GetMeshData().index() == 0) ? model_mesh->num_vertex : own_mesh->points.size(); };

public: 
	ownMeshData* own_mesh = nullptr;
protected:
	ModelMeshData* model_mesh = nullptr;  


	friend class SmileFBX; 
};