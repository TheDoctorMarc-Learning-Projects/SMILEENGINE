#pragma once

#include "Component.h"

#include "parshapes/par_shapes.h"
#include "DevIL/include/IL/il.h"
#include "glmath.h"

#include "MathGeoLib/include/Geometry/AABB.h"
 
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
	void Enable(); 
	void CleanUp();
	void Draw(); 
	void OnTransform(bool data[3]); // update obb, aabb

	// Getters & Setters
	ModelMeshData* GetMeshData() const { return model_mesh; };
	void SetParent(GameObject* parent) { this->parent = parent; };
	Mesh_Type GetMeshType() const { return meshType; };
	debugData debugData;

private: 
	// Internal Creation
	void GenerateModelMeshFromParShapes(par_shapes_mesh*);
	void GenerateBuffers(); 
	void ComputeSpatialData(); 
	void ReLocateMeshVertices(); 

	// Debug
	void DebugDraw(); 

private:
	ModelMeshData* model_mesh = nullptr; 
	Mesh_Type meshType; 


	friend class GameObject;
	friend class SmileFBX; 
};