#pragma once

#include "Component.h"

#include "parshapes/par_shapes.h"
#include "DevIL/include/IL/il.h"
#include "glmath.h"

enum Mesh_Type
{
	Primitive,
	Model
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

	uint id_texture = 0;
	ILubyte* texture = nullptr;

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
		meshBoundingSphereRadius = sqrt(rad_Vec.x * rad_Vec.x + rad_Vec.y * rad_Vec.y + rad_Vec.y * rad_Vec.y);

		// TODO: bounding box, store the 8 vertices and draw them 

	};

public:

	vec3 GetMeshCenter() const { return meshCenter; };
	double GetMeshSphereRadius() const { return meshBoundingSphereRadius; };

	friend class SmileFBX; 
};

// ----------------------------------------------------------------- [Mesh]
class ComponentMesh : public Component
{
public:
	ComponentMesh(par_shapes_mesh*); 
	ComponentMesh(ModelMeshData*);
	~ComponentMesh();

	// TODO: same logic with components as GameObject (Enable, Disable, Update, CleanUp)
	// TODO: the mesh can either be "OpenGL importer related" or a par_shapes_mesh
	// TODO: use the geometry generator to create a GameObject that cointains a ComponentMesh with the par_shapes
	// TODO: use another constructor to recieve a mesh from the old Mesh structure + bring it to this header 

public: 
    // Just like the GameObject:
	void Enable();
	void Update();
	void Disable();
	void CleanUp();

	// If it has a mesh loaded from an FBX ("model_mesh"): 
	void Draw();
	void AssignTexture(const char* path); 
	void AssignCheckersTexture();  // maybe this could become a generic function that recieves a Glubyte  

	ModelMeshData* GetMeshData() const { return model_mesh; };

private: 
	void FillComponentBuffers();

private:
	// The Mesh component is the only one that, like the GmaeObject, has yet another component buffer 
	std::variant<Component*, std::vector<Component*>> components[COMPONENT_TYPE::MAX_COMPONENT_TYPES]; 
	ModelMeshData* model_mesh = nullptr; 
	par_shapes_mesh* primitive_mesh = nullptr; 

	friend class GameObject;
};