#pragma once

#include "Component.h"
#include "parshapes/par_shapes.h"
#include "DevIL/include/IL/il.h"
#include "glmath.h"

#include "MathGeoLib/include/Geometry/AABB.h"
 
class ResourceMesh;
struct ModelMeshData;
struct ownMeshData; 

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


// ----------------------------------------------------------------- [Mesh]
class ComponentMesh : public Component
{
public:
	ComponentMesh(SmileUUID uid, std::string name = "no name");
	~ComponentMesh();

public: 
	void Enable(); 
	void CleanUp();
	void Draw(); 
	void OnTransform(bool data[3]); // update obb, aabb

	// Getters & Setters
	void SetParent(GameObject* parent) { this->parent = parent; };
	Mesh_Type GetMeshType() const { return meshType; };
	debugData debugData;
	ResourceMesh* GetResourceMesh();

private: 
	void DefaultDraw(ModelMeshData*);
	void OwnDraw(ownMeshData*);
	void DrawBegin(); 
	void DrawEnd(); 

	// Debug
	void DebugDraw(); 

private:
	
	Mesh_Type meshType; 
	SmileUUID myresourceID;

	friend class GameObject;
	friend class SmileFBX; 
};