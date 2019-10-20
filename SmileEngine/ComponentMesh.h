#pragma once

#include "Component.h"

#include "parshapes/par_shapes.h"
#include "DevIL/include/IL/il.h"
#include "glmath.h"

enum Mesh_Type
{
	PRIMITIVE,
	MODEL
};

// todo = more generic 

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
	enum minMaxCoords : uint
	{
		MIN_X,
		MIN_Y,
		MIN_Z,
		MAX_X,
		MAX_Y,
		MAX_Z,
		TOTAL_COORDS
	};

	float minmaxCoords[minMaxCoords::TOTAL_COORDS];
	float boundingBox[8]; // TODO

	vec3 meshCenter;
	double meshBoundingSphereRadius = 0;

	void ComputeMeshSpatialData()
	{
		// center 
		float c_X = (minmaxCoords[minMaxCoords::MIN_X] + minmaxCoords[minMaxCoords::MAX_X]) / 2;
		float c_Y = (minmaxCoords[minMaxCoords::MIN_Y] + minmaxCoords[minMaxCoords::MAX_Y]) / 2;
		float c_Z = (minmaxCoords[minMaxCoords::MIN_Z] + minmaxCoords[minMaxCoords::MAX_Z]) / 2;
		meshCenter = vec3(c_X, c_Y, c_Z);

		// sphere radius = module of -> (distance between opposite vertices) / 2
		vec3 min_Vec(minmaxCoords[minMaxCoords::MIN_X], minmaxCoords[minMaxCoords::MIN_Y], minmaxCoords[minMaxCoords::MIN_Z]);
		vec3 max_Vec(minmaxCoords[minMaxCoords::MAX_X], minmaxCoords[minMaxCoords::MAX_Y], minmaxCoords[minMaxCoords::MAX_Z]);
		vec3 rad_Vec = (max_Vec - min_Vec) / 2;
		meshBoundingSphereRadius = (double)sqrt(rad_Vec.x * rad_Vec.x + rad_Vec.y * rad_Vec.y + rad_Vec.y * rad_Vec.y);

		// TODO: bounding box, store the 8 vertices and draw them 

	};

public:

	vec3 GetMeshCenter() const { return meshCenter; };
	double GetMeshSphereRadius() const { return meshBoundingSphereRadius; };

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
    // Just like the GameObject:
	bool AddComponent(Component* comp);

	void Enable();
	void Update();
	void Disable();
	void CleanUp();

	void OnTransform(); 

	std::variant<Component*, std::vector<Component*>> GetComponent(COMPONENT_TYPE type) const
	{
		return components[type];
	}

	// If it has a mesh loaded from an FBX ("model_mesh"): 
	void Draw(); 

	ModelMeshData* GetMeshData() const { return model_mesh; };

private: 
	// components array
	void FillComponentBuffers();

	//generate a mesh form a par shapes
	void GenerateModelMeshFromParShapes(par_shapes_mesh*);
	void ComputeSpatialData(); 
	void GenerateBuffers(); 

	// debug
	void DebugDraw(); 

public: 
	// Assign & Get data
	void SetParent(GameObject* parent) { this->parent = parent; };
	void SetName(std::string name) { this->name = name; };
	GameObject* GetParent() const { return std::get<GameObject*>(parent); };
	std::string GetName() const { return name; };

private:
	// The Mesh component is the only one that, like the GmaeObject, has yet another component buffer 
	std::variant<Component*, std::vector<Component*>> components[COMPONENT_TYPE::MAX_COMPONENT_TYPES]; 
	ModelMeshData* model_mesh = nullptr; 
	Mesh_Type meshType; 
    std::string name; 

	friend class GameObject;
	friend class SmileFBX; 
};