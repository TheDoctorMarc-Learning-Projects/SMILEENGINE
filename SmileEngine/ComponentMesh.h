#pragma once

#include "Component.h"

#include "parshapes/par_shapes.h"
#include "DevIL/include/IL/il.h"
#include "glmath.h"
#include "BoundingData.h"
 
enum Mesh_Type
{
	PRIMITIVE,
	MODEL
};

struct debugData
{
	bool faceNormals = false; 
	bool vertexNormals = false; 
	bool outilineMesh = false; 
	bool outlineParent = false; 
	bool AABB = true; 
	bool OBB = false; 
};

// TODO = more generic 
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

public: 
	bounding::BoundingBox GetOBB() const { return OBB; };
	bounding::BoundingBox GetAABB() const { return AABB; };

private:
	// Bounding boxes
	bounding::BoundingBox AABB; 
	bounding::BoundingBox OBB;
	bool computedData = false; 
	void ComputeMeshSpatialData(); 

public:
	float3 GetMeshCenter() const { return OBB.center; }; 
	double GetMeshSphereRadius() const { return OBB.boundingSphereRadius; };
	std::array<float, minMaxCoords::TOTAL_COORDS> GetMinMaxCoords() const { return OBB.minmaxCoords; };

	friend class SmileFBX;
	friend class ComponentMesh; 
};

// ----------------------------------------------------------------- [Mesh]
class ComponentMesh : public Component
{
public:
	ComponentMesh(par_shapes_mesh*, std::string name = "no name"); 
	ComponentMesh(ModelMeshData*, std::string name = "no name");
	~ComponentMesh();

public: 

	void Update();
	void CleanUp();

	// If it has a mesh loaded from an FBX ("model_mesh"): 
	void Draw(); 
	void OnSelect(bool select);
	ModelMeshData* GetMeshData() const { return model_mesh; };

private: 

	//generate a mesh form a par shapes
	void GenerateModelMeshFromParShapes(par_shapes_mesh*);
	void ComputeSpatialData(); 
	void GenerateBuffers(); 

	// debug
	void DebugDraw(); 


public: 
	// Assign & Get data
	void SetParent(GameObject* parent) { this->parent = parent; };
	Mesh_Type GetMeshType() const { return meshType; }; 
	debugData debugData;

private:
	ModelMeshData* model_mesh = nullptr; 
	Mesh_Type meshType; 


	friend class GameObject;
	friend class SmileFBX; 
};