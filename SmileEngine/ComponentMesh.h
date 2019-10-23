#pragma once

#include "Component.h"

#include "parshapes/par_shapes.h"
#include "DevIL/include/IL/il.h"
#include "glmath.h"

#include <array>

enum Mesh_Type
{
	PRIMITIVE,
	MODEL
};

// TODO = more generic 
struct ModelMeshData
{
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

	// AABB
private:
	std::array<float, minMaxCoords::TOTAL_COORDS> minmaxCoords;
	vec3 meshCenter;
	double meshBoundingSphereRadius = 0;
	bool computedData = false; 
	void ComputeMeshSpatialData(); 

public:

	vec3 GetMeshCenter() const { return meshCenter; }; // this will only work at the beginning 
	double GetMeshSphereRadius() const { return meshBoundingSphereRadius; };
	std::array<float, minMaxCoords::TOTAL_COORDS> GetMinMaxCoords() const { return minmaxCoords; };

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


private:
	ModelMeshData* model_mesh = nullptr; 
	Mesh_Type meshType; 

	friend class GameObject;
	friend class SmileFBX; 
};